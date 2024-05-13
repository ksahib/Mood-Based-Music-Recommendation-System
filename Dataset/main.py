import os
import time
import pandas as pd
import spotipy
from spotipy.exceptions import SpotifyException
from spotipy.oauth2 import SpotifyClientCredentials

# Spotify API credentials
client_id = 'b1a42b6b9b2d4e66aca878db685548a4'
client_secret = 'afa242def70040429fff1fdde235f16c'

# Initialize Spotipy client
client_credentials_manager = SpotifyClientCredentials(client_id=client_id, client_secret=client_secret)
sp = spotipy.Spotify(client_credentials_manager=client_credentials_manager)


def get_audio_features(track_id):
    """
    Get audio features of a track by its ID
    """
    features = ['danceability', 'energy', 'key', 'loudness', 'mode', 'speechiness',
                'acousticness', 'instrumentalness', 'liveness', 'valence', 'tempo',
                'time_signature']
    track = sp.audio_features(tracks=[track_id])[0]
    return {x: track[x] for x in features}


def get_audio_features_with_backoff(track_ids, max_retries=100):
    """
    Get audio features of tracks by their IDs with backoff based on Retry-After header.
    """
    track_ids = [str(track_id) for track_id in track_ids]  # Ensure all track IDs are strings
    features = []
    for attempt in range(max_retries):
        try:
            print(attempt)
            features = sp.audio_features(tracks=track_ids)
            return features
        except SpotifyException as e:
            if e.http_status == 429:  # Rate limit exceeded
                retry_after = int(
                    e.headers.get('Retry-After', 5))  # Default to 5 seconds if Retry-After header is missing
                print(f"Rate limit exceeded. Waiting for {retry_after} seconds before retrying...")
                time.sleep(retry_after)
            else:
                raise  # Re-raise other exceptions
    raise Exception("Max retries reached. Unable to fetch audio features.")


def extract_playlist_features(playlist_id):
    """
    Extract audio features of all tracks in a playlist
    """
    try:
        playlist = sp.playlist_items(playlist_id=playlist_id, additional_types=('track',))
    except SpotifyException as e:
        # Log the error or provide feedback
        print(f"Failed to access playlist with ID {playlist_id}: {e}")
        return []  # Return an empty list of features if unable to access playlist

    track_features = []
    track_ids = []
    for item in playlist['items']:
        if item['track'] is not None:
            track_id = item['track']['id']
            track_ids.append(track_id)

    # Get audio features with backoff
    if track_ids:
        features = get_audio_features_with_backoff(track_ids)
        for item, feature in zip(playlist['items'], features):
            if item['track'] is not None:
                track_name = item['track']['name']
                if feature is not None:  # Check if feature is not None
                    feature['track_name'] = track_name
                    track_features.append(feature)

    return track_features


def write_to_csv(audio_features, file_path):
    """
    Write audio features to a CSV file
    """
    # Check if the file exists
    file_exists = os.path.exists(file_path)

    df = pd.DataFrame(audio_features)
    columns = ['track_name'] + [col for col in df.columns if col != 'track_name']
    df = df[columns]

    # Determine whether to include the header in the CSV file
    header = not file_exists

    df.to_csv(file_path, mode='a', index=False, header=header)


def convert_csv_to_txt(csv_file_path, txt_file_path, columns_to_write):
    """
    Convert CSV file containing audio features to a text file with specified format
    """
    with open(csv_file_path, 'r', encoding='utf-8') as csv_file:  # Specify the encoding as utf-8
        lines = csv_file.readlines()
        header = lines[0].strip().split(',')  # Extract column names from CSV header
        track_name_index = header.index('track_name')
        columns = [header.index(col) for col in columns_to_write]

        # Remove header line and iterate over remaining lines
        lines = lines[1:]
        with open(txt_file_path, 'w', encoding='utf-8') as txt_file:  # Specify the encoding as utf-8
            for line in lines:
                values = line.strip().split(',')
                track_name = values[track_name_index]
                feature_values = ' '.join(values[i] for i in columns)
                txt_line = f"{track_name}, {feature_values}\n"
                txt_file.write(txt_line)


def read_file(file_path):
    playlist_uris = []
    with open(file_path, 'r') as f:
        uri = f.readlines()
    for u in uri:
        u = u.strip()
        playlist_uris.append(u)
    return playlist_uris


if __name__ == "__main__":
    file = 'playlists.txt'
    # Spotify playlist ID
    playlist_id = read_file(file)
    all_tracks = []
    # Extract audio features
    for playlist in playlist_id:
        audio_features = extract_playlist_features(playlist)
        all_tracks.extend(audio_features)

    # Write to CSV file
    write_to_csv(all_tracks, 'playlist_audio_features.csv')
    convert_csv_to_txt('playlist_audio_features.csv', 'playlist_audio_features.txt',
                       ['danceability', 'energy', 'valence', 'loudness'])

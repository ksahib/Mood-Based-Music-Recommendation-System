def remove_duplicates(input_file, output_file):
    with open(input_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    # Remove duplicates
    unique_lines = list(set(lines))

    with open(output_file, 'w', encoding='utf-8') as f:
        for line in unique_lines:
            f.write(line)


if __name__ == "__main__":
    input_file = 'playlist_audio_features.txt'
    output_file = 'playlist_audio_features(pruned).txt'
    remove_duplicates(input_file, output_file)

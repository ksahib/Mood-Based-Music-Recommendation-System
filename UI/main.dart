import 'package:flutter/material.dart';

void main() => runApp(MaterialApp(
  home: Home(),
  theme: new ThemeData(scaffoldBackgroundColor: const Color.fromARGB(255, 36, 35, 35)),
));


class Home extends StatelessWidget {
  @override
  Widget build(BuildContext context)
  {
    return Scaffold(
    appBar: AppBar(
      title: Text('app'),
      backgroundColor: Colors.black,
    ),
    body: Row(
        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
        children: <Widget> [
           GestureDetector(
            onTap: () {
              // Navigate to suggestionWindowCyan
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => suggestionWindowRed()),
              );
            },
            child: Center(
              child: Container(
                color: Colors.red,
                margin: const EdgeInsets.all(10.0),
                width: 150,
                height: 150,
              ),
            ),
          ),
             GestureDetector(
            onTap: () {
              // Navigate to suggestionWindowCyan
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => suggestionWindowCyan()),
              );
            },
            child: Container(
              color: Colors.cyan,
              margin: const EdgeInsets.all(10.0),
              width: 150,
              height: 150,
            ),
          ),
             
              GestureDetector(
            onTap: () {
              // Navigate to suggestionWindowCyan
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => suggestionWindowBlue()),
              );
            },
            child: Container(
              color: Colors.blue[900],
              margin: const EdgeInsets.all(10.0),
              width: 150,
              height: 150,
            ),
          ),
          
        ],
      )
    );
  }
}


class suggestionWindowCyan extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Elevated Square'),
        backgroundColor: Colors.black,
      ),
      body: Stack(
        children: <Widget>[
          // Full-screen background gradient
          Container(
            width: double.infinity,
            height: double.infinity,
            decoration: BoxDecoration(
              gradient: LinearGradient(
                begin: Alignment(1,-1.5),
                end: Alignment(1,1),
                colors: [
                  Colors.cyan,
                  Colors.black,
                ],
              ),
            ),
          ),
          
          // Elevated square centered on top of the background
          
            Card(
              margin: EdgeInsets.all(25.0),
              elevation: 10, // Adds elevation (shadow)
              shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(10.0), // Rounded corners
              ),
              child: Container(
                width: 150, // Width of the square
                height: 150, // Height of the square
                decoration: BoxDecoration(
                  color: Colors.cyan, // Background color of the square
                  borderRadius: BorderRadius.circular(8), // Same as card's border-radius
                ),
            ),
          ),
        ],
      ),
    );
  }
}

class suggestionWindowRed extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Elevated Square'),
        backgroundColor: Colors.black,
      ),
      body: Stack(
        children: <Widget>[
          // Full-screen background gradient
          Container(
            width: double.infinity,
            height: double.infinity,
            decoration: BoxDecoration(
              gradient: LinearGradient(
                begin: Alignment(1,-1.5),
                end: Alignment(1,1),
                colors: [
                  Colors.red,
                  Colors.black,
                ],
              ),
            ),
          ),
          
          // Elevated square centered on top of the background
          
            Card(
              margin: EdgeInsets.all(25.0),
              elevation: 10, // Adds elevation (shadow)
              shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(10.0), // Rounded corners
              ),
              child: Container(
                width: 150, // Width of the square
                height: 150, // Height of the square
                decoration: BoxDecoration(
                  color: Colors.red, // Background color of the square
                  borderRadius: BorderRadius.circular(8), // Same as card's border-radius
                ),
            ),
          ),
        ],
      ),
    );
  }
}

class suggestionWindowBlue extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Elevated Square'),
        backgroundColor: Colors.black,
      ),
      body: Stack(
        children: <Widget>[
          // Full-screen background gradient
          Container(
            width: double.infinity,
            height: double.infinity,
            decoration: BoxDecoration(
              gradient: LinearGradient(
                begin: Alignment(1,-1.5),
                end: Alignment(1,1),
                colors: [
                  const Color.fromARGB(255, 13, 71, 161),
                  Colors.black,
                ],
              ),
            ),
          ),
          
          // Elevated square centered on top of the background
          
            Card(
              margin: EdgeInsets.all(25.0),
              elevation: 10, // Adds elevation (shadow)
              shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(10.0), // Rounded corners
              ),
              child: Container(
                width: 150, // Width of the square
                height: 150, // Height of the square
                decoration: BoxDecoration(
                  color: Colors.blue[900], // Background color of the square
                  borderRadius: BorderRadius.circular(8), // Same as card's border-radius
                ),
            ),
          ),
        ],
      ),
    );
  }
}

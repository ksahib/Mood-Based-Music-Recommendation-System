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
          Center(
            child: Container(
              margin: const EdgeInsets.all(10.0),
              color: Colors.red,
              width: 150,
              height: 150,
            ),
          ),
          Container(
            margin: const EdgeInsets.all(10.0),
            color: Colors.cyan,
            width: 150,
            height: 150,
          ),
          Container(
            margin: const EdgeInsets.all(10.0),
            color: const Color.fromARGB(255, 96, 125, 139),
            width: 150,
            height: 150,
          )
        ],
      )
    );
  }
}
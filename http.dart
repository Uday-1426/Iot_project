import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'D153 Room Grid',
      theme: ThemeData(primarySwatch: Colors.blue),
      home: GridScreen(),
    );
  }
}

class GridScreen extends StatelessWidget {
  final List<String> gridLabels = [
    for (var row in 'ABCDEFGHI'.split(''))
      for (var col in List.generate(9, (index) => index + 1).map((e) => e.toString().padLeft(2, '0')))
        '$row$col'
  ];

  Future<void> sendMoveCommand(String label) async {
    final response = await http.get(Uri.parse('http://<ESP32_IP>/move?label=$label'));
    if (response.statusCode == 200) {
      print('Response: ${response.body}');
    } else {
      print('Failed to send command');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('D153 Room Grid'),
        centerTitle: true,
      ),
      body: SingleChildScrollView(
        child: Column(
          children: List.generate(9, (rowIndex) {
            return Container(
              child: Row(
                mainAxisAlignment: MainAxisAlignment.center, // Center the row
                children: List.generate(9, (colIndex) {
                  int index = rowIndex * 9 + colIndex;

                  // Calculate the screen width and height to determine the button size
                  double buttonWidth = MediaQuery.of(context).size.width / 9 - 10; // Adjust the button width
                  double buttonHeight = MediaQuery.of(context).size.height / 12 - 10; // Adjust the button height

                  return Expanded(
                    child: Padding(
                      padding: const EdgeInsets.all(4.0),
                      child: ElevatedButton(
                        style: ElevatedButton.styleFrom(
                          padding: EdgeInsets.zero,
                          minimumSize: Size(buttonWidth, buttonHeight), // Set the button size dynamically
                        ),
                        onPressed: () {
                          sendMoveCommand(gridLabels[index]);
                          ScaffoldMessenger.of(context).showSnackBar(SnackBar(
                            content: Text('Selected ${gridLabels[index]}'),
                          ));
                        },
                        child: Center(
                          child: Text(
                            gridLabels[index],
                            style: TextStyle(fontSize: 12, color: Colors.black), // Adjust font size
                          ),
                        ),
                      ),
                    ),
                  );
                }),
              ),
            );
          }),
        ),
      ),
    );
  }
}

import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'DisplayPage.dart';  // Import the new page

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'D153 Room Grid',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
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

  Future<void> sendGridSelection(String gridLabel) async {
    final uri = Uri.parse("http://192.168.139.224/button?grid=$gridLabel");  // Replace <ESP32_IP_ADDRESS> with actual IP
    try {
      final response = await http.get(uri);
      if (response.statusCode == 200) {
        print("Sent $gridLabel to ESP32");
      } else {
        print("Failed to send data: ${response.statusCode}");
      }
    } catch (e) {
      print("Error: $e");
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('D153 Room Grid'),
        centerTitle: true,
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: List.generate(9, (rowIndex) {
            return Row(
              children: List.generate(9, (colIndex) {
                int index = rowIndex * 9 + colIndex;
                return Expanded(
                  child: Padding(
                    padding: const EdgeInsets.all(4.0),
                    child: ElevatedButton(
                      style: ElevatedButton.styleFrom(
                        padding: EdgeInsets.zero,
                        minimumSize: Size(50, 50),
                        elevation: 2,
                      ),
                      onPressed: () {
                        sendGridSelection(gridLabels[index]);
                        // Navigate to DisplayPage and pass the selected grid label
                        Navigator.push(
                          context,
                          MaterialPageRoute(
                            builder: (context) => DisplayPage(gridLabel: gridLabels[index]),
                          ),
                        );
                      },
                      child: Center(
                        child: Text(
                          gridLabels[index],
                          style: TextStyle(
                            fontSize: 16,
                            fontWeight: FontWeight.bold,
                            color: Colors.black,
                          ),
                        ),
                      ),
                    ),
                  ),
                );
              }),
            );
          }),
        ),
      ),
    );
  }
}

import 'package:flutter/material.dart';

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

  @override
  Widget build(BuildContext context) {
    // Get screen pixel density (DPI)
    double dpi = MediaQuery.of(context).devicePixelRatio * 160;  // Assuming the base DPI is 160 (MDPI)

    // Convert mm to logical pixels based on DPI
    double buttonWidth = 0.3 * dpi / 25.4;  // 0.3mm converted to pixels
    double buttonHeight = 0.8 * dpi / 25.4;  // 0.8mm converted to pixels

    return Scaffold(
      appBar: AppBar(
        title: Text('D153 Room Grid'),
        centerTitle: true, // Center the title in the AppBar
      ),
      body: SingleChildScrollView(
        child: Column(
          children: List.generate(9, (rowIndex) {
            return Container(
              child: Row(
                children: List.generate(9, (colIndex) {
                  int index = rowIndex * 9 + colIndex; // Calculate the label index
                  return Expanded(  // Use Expanded to prevent overflow
                    child: Padding(
                      padding: const EdgeInsets.all(4.0),
                      child: ElevatedButton(
                        style: ElevatedButton.styleFrom(
                          padding: EdgeInsets.zero,
                          minimumSize: Size(buttonWidth, buttonHeight), // Set button size
                        ),
                        onPressed: () {
                          ScaffoldMessenger.of(context).showSnackBar(
                            SnackBar(content: Text('Selected ${gridLabels[index]}')),
                          );
                        },
                        child: Center(  // Center the label inside the button
                          child: Text(
                            gridLabels[index],
                            style: TextStyle(fontSize: 16, color: Colors.black),
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

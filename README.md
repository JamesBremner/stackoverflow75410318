An application that hunts for and connects together specified sequences of numbers in a 2D matrix

Each sequence is scored with a value.

Maximum length of the path in the graph. The number of picked cells must not exceed this value.

At any given moment, you can only choose cells in a specific column or row.

On each turn, you need to switch between column and row and stay on the same line as the last cell you picked. You have to move at right angles.

Always start with picking the first cell from the top row, then go vertically down to pick the second cell, and then continue switching between column and row as usual.

You can't choose the same cell twice. The resulting path must not contain duplicated cells.

![image](https://user-images.githubusercontent.com/1078056/244896160-4554dc28-ae79-490c-aa39-8f8fd68a9ed7.png)

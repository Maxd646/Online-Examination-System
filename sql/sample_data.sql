-- Sample Data for Online Examination System
-- Insert sample questions into the general question bank

-- DSA Questions
INSERT OR IGNORE INTO questions (subject, question_text, option1, option2, option3, option4, correct_answer, difficulty, explanation, created_by) VALUES
('DSA', 'What is the time complexity of binary search?', 'O(n)', 'O(log n)', 'O(n²)', 'O(1)', 1, 'Medium', 'Binary search divides the search space in half each time, resulting in O(log n) complexity.', 1),
('DSA', 'Which data structure uses LIFO principle?', 'Queue', 'Stack', 'Array', 'Linked List', 1, 'Easy', 'Stack follows Last In First Out (LIFO) principle.', 1),
('DSA', 'What is the worst-case time complexity of insertion sort?', 'O(n)', 'O(n log n)', 'O(n²)', 'O(1)', 2, 'Medium', 'Insertion sort has O(n²) worst-case complexity when the array is reverse sorted.', 1),
('DSA', 'Which traversal visits nodes in ascending order in a BST?', 'Preorder', 'Inorder', 'Postorder', 'Level order', 1, 'Medium', 'Inorder traversal of BST visits nodes in ascending order.', 1),
('DSA', 'What is the space complexity of merge sort?', 'O(1)', 'O(log n)', 'O(n)', 'O(n²)', 2, 'Hard', 'Merge sort requires O(n) additional space for merging.', 1),

-- OOP Questions  
('OOP', 'What is encapsulation in OOP?', 'Inheritance', 'Data hiding', 'Polymorphism', 'Abstraction', 1, 'Easy', 'Encapsulation is the bundling of data and methods that operate on that data.', 1),
('OOP', 'Which keyword is used for inheritance in C++?', 'extends', 'implements', 'public', 'inherits', 2, 'Easy', 'In C++, inheritance is achieved using access specifiers like public, private, protected.', 1),
('OOP', 'What is method overloading?', 'Same method name, different parameters', 'Different method name, same parameters', 'Runtime polymorphism', 'Virtual functions', 0, 'Medium', 'Method overloading allows multiple methods with the same name but different parameters.', 1),
('OOP', 'What is a virtual function?', 'Static function', 'Function that can be overridden', 'Private function', 'Constructor', 1, 'Medium', 'Virtual functions enable runtime polymorphism and can be overridden in derived classes.', 1),
('OOP', 'What is the diamond problem?', 'Memory leak', 'Multiple inheritance ambiguity', 'Stack overflow', 'Null pointer', 1, 'Hard', 'Diamond problem occurs in multiple inheritance when a class inherits from two classes that have a common base class.', 1),

-- COA Questions
('COA', 'What does CPU stand for?', 'Central Processing Unit', 'Computer Processing Unit', 'Central Program Unit', 'Computer Program Unit', 0, 'Easy', 'CPU stands for Central Processing Unit.', 1),
('COA', 'Which is faster: cache or RAM?', 'Cache', 'RAM', 'Both same', 'Depends on size', 0, 'Easy', 'Cache memory is much faster than RAM but smaller in size.', 1),
('COA', 'What is pipelining in CPU?', 'Parallel execution', 'Sequential execution', 'Overlapping instruction execution', 'Memory management', 2, 'Medium', 'Pipelining allows overlapping execution of multiple instructions to improve throughput.', 1),
('COA', 'What is virtual memory?', 'Physical RAM', 'Extended memory using disk', 'Cache memory', 'Register memory', 1, 'Medium', 'Virtual memory extends physical memory by using disk space as additional memory.', 1),
('COA', 'What is the purpose of ALU?', 'Memory management', 'Arithmetic and logic operations', 'Input/output control', 'Instruction fetching', 1, 'Easy', 'ALU (Arithmetic Logic Unit) performs arithmetic and logical operations.', 1),

-- Mathematics Questions
('Mathematics', 'What is 2^10?', '512', '1024', '256', '2048', 1, 'Easy', '2^10 = 1024', 1),
('Mathematics', 'What is log₂(8)?', '2', '3', '4', '8', 1, 'Easy', 'log₂(8) = 3 because 2³ = 8', 1),
('Mathematics', 'What is the derivative of x²?', 'x', '2x', '2', 'x²', 1, 'Medium', 'The derivative of x² is 2x using the power rule.', 1),
('Mathematics', 'What is 5! (5 factorial)?', '25', '120', '100', '60', 1, 'Easy', '5! = 5 × 4 × 3 × 2 × 1 = 120', 1),
('Mathematics', 'What is the sum of angles in a triangle?', '90°', '180°', '270°', '360°', 1, 'Easy', 'The sum of angles in any triangle is always 180°.', 1);
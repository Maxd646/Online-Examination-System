-- Sample questions for testing the Online Examination System

-- Mathematics Questions
INSERT OR IGNORE INTO questions (subject, question_text, option1, option2, option3, option4, correct_answer, difficulty, explanation) VALUES
('Mathematics', 'What is 15 + 27?', '40', '42', '44', '46', 1, 'Easy', '15 + 27 = 42'),
('Mathematics', 'What is 8 × 7?', '54', '56', '58', '60', 1, 'Easy', '8 × 7 = 56'),
('Mathematics', 'What is the square of 9?', '72', '81', '90', '99', 1, 'Medium', '9² = 9 × 9 = 81'),
('Mathematics', 'What is 144 ÷ 12?', '10', '11', '12', '13', 2, 'Medium', '144 ÷ 12 = 12'),
('Mathematics', 'What is the value of π (pi) approximately?', '3.14', '2.71', '1.41', '1.73', 0, 'Easy', 'π ≈ 3.14159...'),

-- Computer Science Questions
('Computer Science', 'What does HTML stand for?', 'HyperText Markup Language', 'High Tech Modern Language', 'Home Tool Markup Language', 'Hyperlink and Text Markup Language', 0, 'Easy', 'HTML stands for HyperText Markup Language'),
('Computer Science', 'Which programming language is known as the "mother of all languages"?', 'Java', 'C', 'Python', 'Assembly', 1, 'Medium', 'C is often called the mother of all programming languages'),
('Computer Science', 'What is the time complexity of binary search?', 'O(n)', 'O(log n)', 'O(n²)', 'O(1)', 1, 'Hard', 'Binary search has O(log n) time complexity'),
('Computer Science', 'Which data structure uses LIFO principle?', 'Queue', 'Stack', 'Array', 'Linked List', 1, 'Medium', 'Stack follows Last In First Out (LIFO) principle'),
('Computer Science', 'What does SQL stand for?', 'Structured Query Language', 'Simple Query Language', 'Standard Query Language', 'System Query Language', 0, 'Easy', 'SQL stands for Structured Query Language'),

-- Science Questions
('Science', 'What is the chemical formula for carbon dioxide?', 'CO2', 'CO', 'C2O', 'O2C', 0, 'Easy', 'Carbon dioxide has the formula CO2'),
('Science', 'Which planet is known as the Red Planet?', 'Venus', 'Mars', 'Jupiter', 'Saturn', 1, 'Easy', 'Mars is called the Red Planet due to its reddish appearance'),
('Science', 'What is the speed of light in vacuum?', '300,000 km/s', '150,000 km/s', '450,000 km/s', '600,000 km/s', 0, 'Medium', 'Light travels at approximately 300,000 km/s in vacuum'),
('Science', 'Which gas makes up about 78% of Earth''s atmosphere?', 'Oxygen', 'Nitrogen', 'Carbon Dioxide', 'Argon', 1, 'Medium', 'Nitrogen makes up about 78% of Earth''s atmosphere'),
('Science', 'What is the smallest unit of matter?', 'Molecule', 'Atom', 'Electron', 'Proton', 1, 'Easy', 'An atom is the smallest unit of matter'),

-- General Knowledge Questions
('General Knowledge', 'Which is the largest ocean on Earth?', 'Atlantic Ocean', 'Pacific Ocean', 'Indian Ocean', 'Arctic Ocean', 1, 'Easy', 'The Pacific Ocean is the largest ocean on Earth'),
('General Knowledge', 'Who wrote "Romeo and Juliet"?', 'Charles Dickens', 'William Shakespeare', 'Jane Austen', 'Mark Twain', 1, 'Easy', 'William Shakespeare wrote Romeo and Juliet'),
('General Knowledge', 'Which country is known as the Land of the Rising Sun?', 'China', 'Japan', 'South Korea', 'Thailand', 1, 'Easy', 'Japan is known as the Land of the Rising Sun'),
('General Knowledge', 'What is the capital of Australia?', 'Sydney', 'Melbourne', 'Canberra', 'Perth', 2, 'Medium', 'Canberra is the capital city of Australia'),
('General Knowledge', 'Which is the longest river in the world?', 'Amazon River', 'Nile River', 'Mississippi River', 'Yangtze River', 1, 'Medium', 'The Nile River is generally considered the longest river in the world');
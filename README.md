# Inverse_Kenematics-Dec2020

When right click on the program, a menu will pop out. For this project, only the first three menu will be important: 
- **Node’s Angle:** You can choose to change the angle of a specific node, when chose, you will be prompt a input for the new angle on the terminal.
- **IK Method:** Choose to use CCD or SVD method for Inverse Kinematic.
- **Kinematic Mode:** Choose to use Forward or Inverse Kinematic.

![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/4c7aaf4f-da74-4c82-a134-dd26d6d4bb21)

|![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/1ab2bc81-6391-45f5-a712-5807f68bafbf)|![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/949cfdf8-02f5-4bdf-a789-c8e118aedebc)|![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/f39bab89-f89f-4961-a116-db065c1598de)|
|-|-|-|

## CCD (Cyclic Coordinate Descent) Method

|Target Point Classification| Results |
|-|-|
|Reachable target point|![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/a3151ddd-ec49-42e4-8e6e-02abecee9d86) ![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/e6e9bb42-39fd-4739-a9da-13cb202676f8) ![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/d91f5785-c35a-4e98-b1ab-fec9fbf59758)|
|Unreachable target point|![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/8f8ff632-7ccc-4348-8b9e-0fcdcaf30d3d) ![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/69d74503-0f58-4022-b45d-b8c0466c321c)|

## SVD (singular value decomposition) Method

|Target Point Classification| Results |
|-|-|
|Reachable target point|![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/2b2ce128-8893-4315-a6a4-39537c2e884c)|
|Unreachable target point|![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/76d37044-74ae-4be3-b8bb-3263357e6725) ![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/e94be55f-9b86-48c1-acf3-9993b91d2255) ![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/7bc91efd-6c09-4c81-98be-c5276cf89507)|

## Brute force test

Here are the solutions of the two methods with more than 100 arms:
|Method Type| Results |
|-|-|
|CCD|![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/e604eed5-938e-4d63-8776-5c6c498f6471)![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/f21d5ba5-b704-47cb-8221-551ebb709b8f)|
|SVD|![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/9ba67d2a-fc61-4d8c-a697-9750da6001e1)![image](https://github.com/yuantianle/Inverse_Kenematic-Dec2020/assets/61530469/2d4107bd-a388-4634-bc9b-44955e94e966)|

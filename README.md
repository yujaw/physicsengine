# Physics Engine in C

## SHAPES
**Implemented three different shapes including**
- **Ellipse**
    - Ellipse can have different radius for x and y axis ___or same radius for a circle___
- **Polygon**
    - Polygon can have more than 3 sides
    - Each vertex is stored as a vector ___i.e. x and y axis___
- **Line**
    - Line is a shape with two vectors ___a starting point and an ending point___

## Collision
**Used Gilbert Johnson Keerthi _GJK_ Algorithm for Collision Detection and Expanding Polytope Algorithm _EPA_ for Collision Data**

## Ongoing Works
**Currently working on implementing multi-particle collision system**

## ToDo
- **Properly produce shape even when the coords is not in order**
    - Convex Hull
- **Decompose concave shapes into group of convex shapes**
- **Use K-D tree to optimize collisions between bodies**

<br/>

---

**<p align="right">Made by Yujaw Manandhar</p>**
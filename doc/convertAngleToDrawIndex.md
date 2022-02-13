# Angles, D2TM angles and converting to drawing index


# Degrees between two points
D2TM considers 2 'points', a 'from' (the center of an imaginary cirlce) and
a 'to' which is at the edge of the circle.

Given these two points you get a `degrees`. You get this by calling `fDegrees`.

It takes `x1,y1` and `x2,y2`. Where `x1,y1` is always considered to be the 'from', ie
center of the 'circle'.

# Determining 'facing angle'
Once we have this degrees value you can convert this to "angles", which
are basicallys segments within that circle to determine what the 'facing angle'
would be for a unit or projectile. Use `faceAngle` for this.

This function takes the amoutn of `degrees` we had from `fDegrees` and
the amount of `angles` (segments) you have in the circle. Assuming 0 degrees is `up`.

```
float degrees = fDegrees(fromX, fromY, toX, toY);
int facingAngle = faceAngle(degrees);
```

When no `angles` are given, `faceAngle` assumes `8`.

# Determining drawing index
Consider this QUAD image:

![](quad_face_angles.png)

The numbers (in yellow) are the D2TM face angles.

The drawing index starts from left to right, meaning the first quad picture means `drawing index 0`. As you can see, it has a `face index` of `2`.

In order to convert between a `faceAngle` to a `drawIndex` on the bitmap you use `convertAngleToDrawIndex`

```
float degrees = fDegrees(fromX, fromY, toX, toY);
int facingAngle = faceAngle(degrees);
int drawIndex = convertAngleToDrawIndex(facingAngle);
```

Without any extra parameters `convertAngleToDrawIndex` assumes that
the bitmap is using pictures going into a counter-clockwise direction (like the QUAD above).
Meaning, when the drawing index goes to the left it will correspond with the facing angle.

To make it even more concrete:

| Facing angle | QUAD draw index |
|--------------|-----------------|
| 0            | 2               |
| 1            | 1               |
| 2            | 0               |
| 3            | 7               |
| 4            | 6               |
| 5            | 5               |
| 6            | 4               |
| 7            | 3               |

As you can see, the left column, the d2tm facing angle goes from 0 to 7.
The QUAD index goes down (from 7 to 0), but it *also* has a different
starting point. The 2nd picture has a QUAD facing upwards. This is called an offset.

By default `convertAngleToDrawIndex` assumes an offset of `2`, and that
the bitmap uses counter-clockwise rotation. You can overrule that if you want.


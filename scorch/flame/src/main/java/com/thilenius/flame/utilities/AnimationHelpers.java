package com.thilenius.flame.utilities;

import com.thilenius.flame.utilities.types.Location3D;
import com.thilenius.flame.utilities.types.LocationF3D;

public class AnimationHelpers {

    public enum FaceDirections {
        North,
        East,
        South,
        West
    }
    public enum AnimationTypes {
        Idle,
        TurnLeft,
        TurnRight,
        Forward,
        Backward,
        Up,
        Down
    }

    public static LocationF3D getRotationVector(FaceDirections faceDirection) {
        LocationF3D facingDirection = new LocationF3D();
        switch (faceDirection) {
            case North: facingDirection = new LocationF3D(0.0f, 0.0f, -1.0f); break;
            case East: facingDirection = new LocationF3D(1.0f, 0.0f, 0.0f); break;
            case South: facingDirection = new LocationF3D(0.0f, 0.0f, 1.0f); break;
            case West: facingDirection = new LocationF3D(-1.0f, 0.0f, 0.0f); break;
        }
        return facingDirection;
    }

    public static FaceDirections getNewFaceDirByAnimation(FaceDirections faceDirection, AnimationTypes animation) {
        switch (animation) {
            case TurnLeft:
                switch (faceDirection) {
                    case North: return FaceDirections.West;
                    case East: return FaceDirections.North;
                    case South: return FaceDirections.East;
                    case West: return FaceDirections.South;
                }
                break;
            case TurnRight:
                switch (faceDirection) {
                    case North: return FaceDirections.East;
                    case East: return FaceDirections.South;
                    case South: return FaceDirections.West;
                    case West: return FaceDirections.North;
                }
                break;
        }
        return faceDirection;
    }

    public static Location3D getBlockFromAction(Location3D location, FaceDirections faceDirection,
                                                AnimationTypes animation) {
        // Moving forward backward
        if (animation == AnimationTypes.Forward || animation == AnimationTypes.Backward) {
            // Construct faceDir vector
            LocationF3D facingDirection = getRotationVector(faceDirection);
            if (animation == AnimationTypes.Backward) {
                facingDirection = facingDirection.scale(-1.0f);
            }
            // Multiple each component by fractionTime
            return new Location3D(location.X + Math.round(facingDirection.X),
                                  location.Y + Math.round(facingDirection.Y),
                                  location.Z + Math.round(facingDirection.Z));
        } else if (animation == AnimationTypes.Up || animation == AnimationTypes.Down) {
            int up = animation == AnimationTypes.Up ? 1 : -1;
            return new Location3D(location.X, location.Y + up, location.Z);
        } else {
            return new Location3D(location.X, location.Y, location.Z);
        }
    }

}

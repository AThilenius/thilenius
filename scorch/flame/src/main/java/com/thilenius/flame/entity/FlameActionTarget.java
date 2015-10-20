package com.thilenius.flame.entity;

import com.thilenius.flame.entity.FlameActionPath;
import com.thilenius.flame.entity.FlameTileEntity;
import com.thilenius.flame.utilities.types.Location3D;

import java.lang.reflect.Method;

public class FlameActionTarget {

    public FlameTileEntity TileEntity;
    public FlameActionPath ActionPath;
    public Method TargetMethod;
    public String EntityUUID;
    public String Username;

    public FlameActionTarget(FlameTileEntity tileEntity, FlameActionPath actionPath, Method targetMethod,
                             String entityUUID, String username) {
        TileEntity = tileEntity;
        ActionPath = actionPath;
        TargetMethod = targetMethod;
        EntityUUID = entityUUID;
        Username = username;
    }

    @Override
    public String toString() {
        return "Entity UUID [ " + EntityUUID + " ], Username [ " + Username + " ], Action Method Name [ " +
                TargetMethod.getName() + " ]";
    }

}

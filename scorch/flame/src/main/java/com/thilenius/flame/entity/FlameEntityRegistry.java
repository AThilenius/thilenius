package com.thilenius.flame.entity;

import com.thilenius.flame.entity.FlameActionTarget;
import com.thilenius.flame.utilities.StringUtils;
import com.thilenius.flame.utilities.types.Location3D;

import java.util.HashMap;
import java.util.HashSet;

// Hacked to register entities 1:1 to usernames for now
public class FlameEntityRegistry {

    private HashMap<String, FlameTileEntity> m_entitiesByUsername = new HashMap<String, FlameTileEntity>();

    public void register (FlameTileEntity entity) {
        m_entitiesByUsername.put(entity.getPlayerUsername(), entity);
        System.out.println("Registering entity with UUID: " + entity.getUuid());
    }

    public void unregister (FlameTileEntity entity) {
        m_entitiesByUsername.remove(entity.getPlayerUsername());
        System.out.println("Unregistering: " + entity.toString());
    }

    public FlameActionTarget getTargetByUsername(String username, String actionName) {
        FlameTileEntity tileEntity = m_entitiesByUsername.get(username);
        if (tileEntity == null) {
            return null;
        }
        return tileEntity.getActionTargetByName(actionName);
    }
}

package com.thilenius.flame.entity;

import com.thilenius.flame.Flame;
import com.thilenius.flame.GlobalData;
import com.thilenius.flame.entity.FlameActionTarget;
import com.thilenius.flame.utilities.StringUtils;
import com.thilenius.flame.utilities.types.Location3D;
import net.minecraft.server.MinecraftServer;
import net.minecraft.world.World;
import net.minecraft.world.WorldServer;

import java.util.HashMap;
import java.util.HashSet;

// Hacked to register entities 1:1 to usernames for now
public class FlameEntityRegistry {

    // In the format USERNAME-DIMENSION_ID
    private HashMap<String, FlameTileEntity> m_entitiesByUsername = new HashMap<String, FlameTileEntity>();

    public void register (FlameTileEntity entity) {
        if (entity.getWorldObj() instanceof WorldServer) {
            String key = entity.getPlayerUsername() + "-" + entity.getWorldObj().provider.dimensionId;
            if (!m_entitiesByUsername.containsKey(key)) {
                m_entitiesByUsername.put(key, entity);
                System.out.println("Registering entity with UUID: " + entity.getUuid() + ", key of: " + key);
            } else {
                System.out.println("Ignoring duplicated registry for entity with UUID: " + entity.getUuid() +
                        ", key of: " + key);
            }
        }
    }

    public void unregister (FlameTileEntity entity) {
        if (entity.getWorldObj() instanceof WorldServer) {
            String key = entity.getPlayerUsername() + "-" + entity.getWorldObj().provider.dimensionId;
            if (m_entitiesByUsername.containsKey(key)) {
                m_entitiesByUsername.remove(key);
                System.out.println("Unregistering: " + entity.toString());
            } else {
                System.out.println("Can't unregister: " + entity.toString() + ". It was never registered.");
            }
        }
    }

    public FlameActionTarget getTargetByUsernameAndDimension(String username, int dimension, String actionName) {
        String key = username + "-" + dimension;
        FlameTileEntity tileEntity = m_entitiesByUsername.get(key);
        if (tileEntity == null) {
            return null;
        }
        return tileEntity.getActionTargetByName(actionName);
    }
}

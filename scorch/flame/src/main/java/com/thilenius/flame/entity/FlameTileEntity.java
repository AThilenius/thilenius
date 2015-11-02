package com.thilenius.flame.entity;

import com.thilenius.flame.Flame;
import net.minecraft.nbt.NBTTagCompound;
import net.minecraft.tileentity.TileEntity;

import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.UUID;

// Does nothing more than add/manage/remove Action Targets
public class FlameTileEntity extends TileEntity {

    private String m_playerUsername;
    private String m_entityUuid = UUID.randomUUID().toString();
    private HashMap<String, FlameActionTarget> m_actionTargetsByName = new HashMap<String, FlameActionTarget>();
    private boolean m_InitCalled = false;

    @Override
    public void writeToNBT(NBTTagCompound nbt)
    {
        super.writeToNBT(nbt);
        if (m_playerUsername != null) {
            // No, fucking, idea, why, this, shit, fucking, game, would, call, this, before, fucking, loading, the, God,
            // Damn, entity... Lord this code is shit.
            nbt.setString("player_username", m_playerUsername);
            nbt.setString("entity_uuid", m_entityUuid);
        }
    }

    @Override
    public void readFromNBT(NBTTagCompound nbt)
    {
        super.readFromNBT(nbt);
        m_playerUsername = nbt.getString("player_username");
        m_entityUuid = nbt.getString("entity_uuid");
    }

    @Override
    public void updateEntity() {
        if (!worldObj.isRemote &&!m_InitCalled && m_playerUsername != null) {
            m_InitCalled = true;
            initialize();
            registerActionPaths();
        }
    }

    public void registerActionPaths() {
        if (!worldObj.isRemote) {
            for (final Method method : this.getClass().getDeclaredMethods()) {
                if (method.isAnnotationPresent(FlameActionPath.class)) {
                    FlameActionPath actionPath = method.getAnnotation(FlameActionPath.class);
                    FlameActionTarget target = new FlameActionTarget(this, actionPath, method, m_entityUuid,
                            getPlayerUsername());
                    if (m_actionTargetsByName.containsKey(actionPath.value())) {
                        System.out.println("!! DUPLICATE ACTION TARGET FOUND !! [ " + actionPath.value() + " ]");
                    }
                    m_actionTargetsByName.put(actionPath.value(), target);
                }
            }
            Flame.Globals.EntityRegistry.register(this);
        }
    }

    public void unregisterActionPaths() {
        if (!worldObj.isRemote) {
            m_actionTargetsByName.clear();
            Flame.Globals.EntityRegistry.unregister(this);
        }
    }

    public String getPlayerUsername() {
        return m_playerUsername;
    }

    public void setPlayerName(String playerName) {
        this.m_playerUsername = playerName;
    }

    public FlameActionTarget getActionTargetByName(String actionName) {
        return m_actionTargetsByName.get(actionName);
    }

    public String getUuid() {
        return m_entityUuid;
    }

    protected void initialize() { }
}
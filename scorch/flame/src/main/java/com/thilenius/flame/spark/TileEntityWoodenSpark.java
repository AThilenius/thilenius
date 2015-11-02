package com.thilenius.flame.spark;

import com.thilenius.flame.tpad.TileEntityTeleportPad;
import com.thilenius.flame.utilities.types.Location3D;
import net.minecraft.item.ItemStack;
import net.minecraft.nbt.NBTTagCompound;
import net.minecraft.nbt.NBTTagList;
import net.minecraft.network.NetworkManager;
import net.minecraft.network.Packet;
import net.minecraft.network.play.server.S35PacketUpdateTileEntity;
import net.minecraft.tileentity.TileEntity;
import net.minecraftforge.common.util.Constants;

public class TileEntityWoodenSpark extends TileEntity {

    private Location3D m_teleportPadLocation = null;
    private boolean m_isSuppressingDrop = false;

    public TileEntityTeleportPad getTeleportPad() {
        if (m_teleportPadLocation == null) {
            return null;
        }
        TileEntity tileEntity = worldObj.getTileEntity(m_teleportPadLocation.X, m_teleportPadLocation.Y,
                m_teleportPadLocation.Z);
        if (tileEntity != null && tileEntity instanceof TileEntityTeleportPad) {
            return (TileEntityTeleportPad) tileEntity;
        } else {
            return null;
        }
    }

    public void setTeleportPadLocation(Location3D teleportPadLocation) {
        m_teleportPadLocation = teleportPadLocation;
    }

    public void suppressDrop() {
        m_isSuppressingDrop = true;
    }

    public boolean isSuppressingDrop() {
        return m_isSuppressingDrop;
    }

    // ======   Network / Disk IO Handling / TileEntity Overrides   ====================================================
    @Override
    public void writeToNBT(NBTTagCompound nbt)
    {
        // Don't write anything if we don't know where the pad is
        TileEntityTeleportPad tileEntityTeleportPad = getTeleportPad();
        if (tileEntityTeleportPad != null) {
            super.writeToNBT(nbt);
            nbt.setString("padLocation", m_teleportPadLocation.toString());
        }
    }

    @Override
    public void readFromNBT(NBTTagCompound nbt)
    {
        super.readFromNBT(nbt);
        setTeleportPadLocation(Location3D.fromString(nbt.getString("padLocation")));
    }

    @Override
    public Packet getDescriptionPacket() {
        NBTTagCompound nbt = new NBTTagCompound();
        writeToNBT(nbt);
        return new S35PacketUpdateTileEntity(this.xCoord, this.yCoord, this.zCoord, 1, nbt);
    }

    @Override
    public void onDataPacket(NetworkManager net, S35PacketUpdateTileEntity pkt) {
        readFromNBT(pkt.func_148857_g());
    }

}
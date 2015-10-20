package com.thilenius.flame.tpad;

import com.thilenius.flame.Flame;
import com.thilenius.flame.utilities.AnimationHelpers;
import com.thilenius.flame.utilities.types.CountdownTimer;
import com.thilenius.flame.utilities.types.Location3D;
import net.minecraft.entity.player.EntityPlayer;
import net.minecraft.inventory.IInventory;
import net.minecraft.item.ItemStack;
import net.minecraft.nbt.NBTTagCompound;
import net.minecraft.nbt.NBTTagList;
import net.minecraft.network.NetworkManager;
import net.minecraft.network.Packet;
import net.minecraft.network.play.server.S35PacketUpdateTileEntity;
import net.minecraft.tileentity.TileEntity;
import net.minecraft.world.World;
import net.minecraftforge.common.util.Constants;

public class WoodenSparkTileEntity extends TileEntity {

    private Location3D m_teleportPadLocation = null;
    private ItemStack[] m_inventory = new ItemStack[9];

    public TeleportPadTileEntity getTeleportPad() {
        TileEntity tileEntity = worldObj.getTileEntity(m_teleportPadLocation.X, m_teleportPadLocation.Y,
                m_teleportPadLocation.Z);
        if (tileEntity != null && tileEntity instanceof TeleportPadTileEntity) {
            return (TeleportPadTileEntity) tileEntity;
        } else {
            return null;
        }
    }

    public void setTeleportPadLocation(Location3D teleportPadLocation) {
        m_teleportPadLocation = teleportPadLocation;
    }

    // ======   Network / Disk IO Handling / TileEntity Overrides   ====================================================
    @Override
    public void writeToNBT(NBTTagCompound nbt)
    {
        // Don't write anything if we don't know where the pad is
        TeleportPadTileEntity teleportPadTileEntity = getTeleportPad();
        if (teleportPadTileEntity != null) {
            super.writeToNBT(nbt);
            nbt.setString("padLocation", m_teleportPadLocation.toString());
            // Inventory
            NBTTagList itemList = new NBTTagList();
            for (int i = 0; i < m_inventory.length; i++) {
                ItemStack stack = m_inventory[i];
                if (stack != null) {
                    NBTTagCompound tag = new NBTTagCompound();
                    tag.setByte("Slot", (byte) i);
                    stack.writeToNBT(tag);
                    itemList.appendTag(tag);
                }
            }
            nbt.setTag("Inventory", itemList);
        }
    }

    @Override
    public void readFromNBT(NBTTagCompound nbt)
    {
        super.readFromNBT(nbt);
        setTeleportPadLocation(Location3D.fromString(nbt.getString("padLocation")));
        // Inventory
        NBTTagList tagList = nbt.getTagList("Inventory", Constants.NBT.TAG_COMPOUND);
        for (int i = 0; i < tagList.tagCount(); i++) {
            NBTTagCompound tag = tagList.getCompoundTagAt(i);
            byte slot = tag.getByte("Slot");
            if (slot >= 0 && slot < m_inventory.length) {
                m_inventory[slot] = ItemStack.loadItemStackFromNBT(tag);
            }
        }
    }

    @Override
    public Packet getDescriptionPacket() {
        NBTTagCompound nbt = new NBTTagCompound();
        writeToNBT(nbt);
        return new S35PacketUpdateTileEntity(this.xCoord, this.yCoord, this.zCoord, 1, nbt);
    }

    @Override
    public void onDataPacket(NetworkManager net, S35PacketUpdateTileEntity pkt) {
        super.onDataPacket(net, pkt);
        this.readFromNBT(pkt.func_148857_g());
    }

}
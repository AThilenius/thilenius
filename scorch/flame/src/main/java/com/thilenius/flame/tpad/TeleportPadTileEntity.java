package com.thilenius.flame.tpad;

import cofh.api.energy.IEnergyReceiver;
import com.fasterxml.jackson.databind.JsonNode;
import com.thilenius.flame.GlobalData;
import com.thilenius.flame.entity.FlameActionPath;
import com.thilenius.flame.entity.FlameActionTargetResponse;
import com.thilenius.flame.entity.FlameTileEntity;
import com.thilenius.flame.utilities.AnimationHelpers;
import com.thilenius.flame.utilities.types.CountdownTimer;
import com.thilenius.flame.utilities.types.Location3D;
import net.minecraft.entity.player.EntityPlayer;
import net.minecraft.inventory.IInventory;
import net.minecraft.item.ItemStack;
import net.minecraft.nbt.NBTTagCompound;
import net.minecraft.network.NetworkManager;
import net.minecraft.network.Packet;
import net.minecraft.network.play.server.S35PacketUpdateTileEntity;
import net.minecraft.server.MinecraftServer;
import net.minecraft.world.World;
import net.minecraftforge.common.util.ForgeDirection;

public class TeleportPadTileEntity extends FlameTileEntity implements IEnergyReceiver, IInventory {

    protected float ANIMATION_TIME = 0.5f;

    private AnimationHelpers.FaceDirections m_sparkFaceDir = AnimationHelpers.FaceDirections.North;
    private AnimationHelpers.AnimationTypes m_sparkAnimation = AnimationHelpers.AnimationTypes.Idle;
    private CountdownTimer m_animationTimer;
    private Location3D m_sparkLocation = null;
    private ItemStack[] m_inventory = new ItemStack[9];

    // ======   Network / Disk IO Handling / TileEntity Overrides   ====================================================
    @Override
    public void writeToNBT(NBTTagCompound nbt)
    {
        super.writeToNBT(nbt);
        nbt.setString("sparkFaceDir", m_sparkFaceDir.name());
        nbt.setString("sparkAnimation", m_sparkAnimation.name());
        nbt.setString("sparkLocation", getSparkLocation().toString());
    }

    @Override
    public void readFromNBT(NBTTagCompound nbt)
    {
        super.readFromNBT(nbt);
        m_sparkFaceDir = AnimationHelpers.FaceDirections.valueOf(nbt.getString("sparkFaceDir"));
        m_sparkAnimation = AnimationHelpers.AnimationTypes.valueOf(nbt.getString("sparkAnimation"));
        m_sparkLocation = Location3D.fromString(nbt.getString("sparkLocation"));
        m_animationTimer = new CountdownTimer(ANIMATION_TIME);
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

    @Override
    public double getMaxRenderDistanceSquared() {
        return 16384.0D;
    }

    // ======   Action Path Handlers   =================================================================================
    @FlameActionPath("move")
    public FlameActionTargetResponse moveAction(JsonNode message) {
        World world = MinecraftServer.getServer().worldServers[0];
        if (m_animationTimer != null && !m_animationTimer.hasElapsed()) {
            return FlameActionTargetResponse.fromOnCoolDown();
        }
        AnimationHelpers.AnimationTypes animationType
                = AnimationHelpers.AnimationTypes.valueOf(message.get("direction").asText());
        if (animationType == null ||
                (animationType != AnimationHelpers.AnimationTypes.Forward &&
                        animationType != AnimationHelpers.AnimationTypes.Backward &&
                        animationType != AnimationHelpers.AnimationTypes.Up &&
                        animationType != AnimationHelpers.AnimationTypes.Down)) {
            return FlameActionTargetResponse.fromOperationFailure("Invalid or missing field [direction]");
        }
        Location3D newLocation = AnimationHelpers.getBlockFromAction(getSparkLocation(), getSparkFaceDirection(),
                animationType);
        // Check if we can move to the new spot
        if (world.isAirBlock(newLocation.X, newLocation.Y, newLocation.Z)) {
            world.setBlock(newLocation.X, newLocation.Y, newLocation.Z, GlobalData.WoodenSparkBlock);
            WoodenSparkTileEntity sparkTileEntity = (WoodenSparkTileEntity) world.getTileEntity(
                    newLocation.X, newLocation.Y, newLocation.Z);
            sparkTileEntity.setTeleportPadLocation(getPadLocation());
            if (!getSparkLocation().equals(getPadLocation())) {
                world.setBlockToAir(getSparkLocation().X, getSparkLocation().Y, getSparkLocation().Z);
            }
            m_sparkLocation = newLocation;
            animateServerAndSendToClients(animationType);
            return FlameActionTargetResponse.fromJson("{\"did_pass\":true}");
        } else {
            return FlameActionTargetResponse.fromJson("{\"did_pass\":false}");
        }
    }

    @FlameActionPath("rotate")
    public void rotateAction(JsonNode message) {

    }

    // ======   Accessors   ============================================================================================
    public CountdownTimer getAnimationTimer() {
        return m_animationTimer;
    }

    public AnimationHelpers.AnimationTypes getSparkAnimationType() {
        return m_sparkAnimation;
    }

    public AnimationHelpers.FaceDirections getSparkFaceDirection() {
        return m_sparkFaceDir;
    }

    public Location3D getSparkLocation() {
        if (m_sparkLocation == null) {
            m_sparkLocation = new Location3D(xCoord, yCoord, zCoord);
        }
        return m_sparkLocation;
    }

    public Location3D getPadLocation() {
        return new Location3D(xCoord, yCoord, zCoord);
    }

    // ======   Helpers   ==============================================================================================
    private void animateServerAndSendToClients(AnimationHelpers.AnimationTypes animationType) {
        m_sparkAnimation = animationType;
        m_sparkFaceDir = AnimationHelpers.getNewFaceDirByAnimation(m_sparkFaceDir, animationType);
        m_animationTimer = new CountdownTimer(ANIMATION_TIME);
        this.worldObj.markBlockForUpdate(getPadLocation().X, getPadLocation().Y, getPadLocation().Z);
        this.worldObj.markBlockForUpdate(getSparkLocation().X, getSparkLocation().Y, getSparkLocation().Z);
    }

    // =================================================================================================================
    // ====  IInventory Implementation  ================================================================================
    // =================================================================================================================
    @Override
    public int getSizeInventory() {
        return m_inventory.length;
    }

    @Override
    public ItemStack getStackInSlot(int slot) {
        return m_inventory[slot];
    }

    @Override
    public ItemStack decrStackSize(int slot, int amt) {
        ItemStack stack = getStackInSlot(slot);
        if (stack != null) {
            if (stack.stackSize <= amt) {
                setInventorySlotContents(slot, null);
            } else {
                stack = stack.splitStack(amt);
                if (stack.stackSize == 0) {
                    setInventorySlotContents(slot, null);
                }
            }
        }
        return stack;
    }

    @Override
    public ItemStack getStackInSlotOnClosing(int slot) {
        ItemStack stack = getStackInSlot(slot);
        if (stack != null) {
            setInventorySlotContents(slot, null);
        }
        return stack;
    }

    @Override
    public void setInventorySlotContents(int slot, ItemStack stack) {
        m_inventory[slot] = stack;
        if (stack != null && stack.stackSize > getInventoryStackLimit()) {
            stack.stackSize = getInventoryStackLimit();
        }
    }

    @Override
    public String getInventoryName() {
        return "flame.teleportPadInv";
    }

    @Override
    public boolean hasCustomInventoryName() {
        return false;
    }

    @Override
    public int getInventoryStackLimit() {
        return 64;
    }

    @Override
    public boolean isUseableByPlayer(EntityPlayer player) {
        return player.getEntityWorld().getTileEntity(xCoord, yCoord, zCoord) == this &&
                player.getDistanceSq(xCoord + 0.5, yCoord + 0.5, zCoord + 0.5) < 64;
    }

    @Override
    public void openInventory() { }

    @Override
    public void closeInventory() { }

    @Override
    public boolean isItemValidForSlot(int p_94041_1_, ItemStack p_94041_2_) {
        return false;
    }

    // =================================================================================================================
    // ====  IEnergyReceiver Implementation  ===========================================================================
    // =================================================================================================================
    @Override
    public int receiveEnergy(ForgeDirection forgeDirection, int i, boolean b) {
        System.out.println("receiveEnergy: " + i + ":" + b);
        return i;
    }

    @Override
    public int getEnergyStored(ForgeDirection forgeDirection) {
        System.out.println("getEnergyStored");
        return 0;
    }

    @Override
    public int getMaxEnergyStored(ForgeDirection forgeDirection) {
        System.out.println("getEnergyStored");
        return 0;
    }

    @Override
    public boolean canConnectEnergy(ForgeDirection forgeDirection) {
        return true;
    }

}
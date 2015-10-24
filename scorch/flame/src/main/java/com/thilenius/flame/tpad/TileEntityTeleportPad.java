package com.thilenius.flame.tpad;

import cofh.api.energy.IEnergyReceiver;
import com.fasterxml.jackson.databind.JsonNode;
import com.thilenius.flame.GlobalData;
import com.thilenius.flame.entity.FlameActionPath;
import com.thilenius.flame.entity.FlameActionTargetResponse;
import com.thilenius.flame.entity.FlameTileEntity;
import com.thilenius.flame.spark.TileEntityWoodenSpark;
import com.thilenius.flame.utilities.AnimationHelpers;
import com.thilenius.flame.utilities.types.CountdownTimer;
import com.thilenius.flame.utilities.types.Location3D;
import net.minecraft.block.Block;
import net.minecraft.entity.item.EntityItem;
import net.minecraft.entity.player.EntityPlayer;
import net.minecraft.init.Blocks;
import net.minecraft.inventory.IInventory;
import net.minecraft.item.Item;
import net.minecraft.item.ItemStack;
import net.minecraft.nbt.NBTTagCompound;
import net.minecraft.network.NetworkManager;
import net.minecraft.network.Packet;
import net.minecraft.network.play.server.S35PacketUpdateTileEntity;
import net.minecraft.server.MinecraftServer;
import net.minecraft.tileentity.TileEntity;
import net.minecraft.world.World;
import net.minecraftforge.common.util.ForgeDirection;

import java.util.ArrayList;
import java.util.Random;

public class TileEntityTeleportPad extends FlameTileEntity implements IEnergyReceiver, IInventory {

    // Used for animation offset
    public final float TIME_OFFSET_CONST = new Random().nextFloat();

    protected float ANIMATION_TIME = 0.5f;

    private AnimationHelpers.FaceDirections m_sparkFaceDir = AnimationHelpers.FaceDirections.North;
    private AnimationHelpers.AnimationTypes m_sparkAnimation = AnimationHelpers.AnimationTypes.Idle;
    private CountdownTimer m_animationTimer;
    private Location3D m_sparkLocation = null;
    private ItemStack[] m_inventory = new ItemStack[9];

    static {
        addMapping(TileEntityTeleportPad.class, "teleportPad");
    }

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
        readFromNBT(pkt.func_148857_g());
    }

    @Override
    public double getMaxRenderDistanceSquared() {
        return 16384.0D;
    }

    // ======   Action Path Handlers   =================================================================================
    @FlameActionPath("move")
    public FlameActionTargetResponse moveAction(World world, JsonNode message) {
        if (m_animationTimer != null && !m_animationTimer.hasElapsed()) {
            return FlameActionTargetResponse.fromOnCoolDown();
        }
        AnimationHelpers.AnimationTypes animationType
                = AnimationHelpers.AnimationTypes.valueOf(message.get("direction").asText());
        if (animationType == null || animationType == AnimationHelpers.AnimationTypes.Idle) {
            return FlameActionTargetResponse.fromOperationFailure("Invalid or missing field [direction]");
        }
        Location3D newLocation = AnimationHelpers.getBlockFromAction(getSparkLocation(), getSparkFaceDirection(),
                animationType);
        if (!newLocation.equals(getSparkLocation())) {
            // Move action, Check if we can move to the new spot
            if (world.isAirBlock(newLocation.X, newLocation.Y, newLocation.Z)) {
                world.setBlock(newLocation.X, newLocation.Y, newLocation.Z, GlobalData.WoodenSparkBlock);
                TileEntityWoodenSpark sparkTileEntity = (TileEntityWoodenSpark) world.getTileEntity(
                        newLocation.X, newLocation.Y, newLocation.Z);
                sparkTileEntity.setTeleportPadLocation(getPadLocation());
                if (!getSparkLocation().equals(getPadLocation())) {
                    TileEntityWoodenSpark tileEntityWoodenSpark = (TileEntityWoodenSpark) world.getTileEntity(
                            getSparkLocation().X, getSparkLocation().Y, getSparkLocation().Z);
                    // Suppress the custom block breaking effect
                    tileEntityWoodenSpark.suppressDrop();
                    world.setBlockToAir(getSparkLocation().X, getSparkLocation().Y, getSparkLocation().Z);
                }
                m_sparkLocation = newLocation;
                animateServerAndSendToClients(world, animationType);
                return FlameActionTargetResponse.fromJson("{\"did_pass\":true}");
            } else {
                return FlameActionTargetResponse.fromJson("{\"did_pass\":false}");
            }
        } else {
            // Rotate action
            animateServerAndSendToClients(world, animationType);
            return FlameActionTargetResponse.fromJson("{\"did_pass\":true}");
        }
    }

    @FlameActionPath("mine")
    public FlameActionTargetResponse mineAction(World world, JsonNode message) {
        AnimationHelpers.AnimationTypes animationType
                = AnimationHelpers.AnimationTypes.valueOf(message.get("direction").asText());
        if (animationType == null || (animationType != AnimationHelpers.AnimationTypes.Up &&
                animationType != AnimationHelpers.AnimationTypes.Down &&
                animationType != AnimationHelpers.AnimationTypes.Forward)) {
            return FlameActionTargetResponse.fromOperationFailure("Invalid or missing field [direction]");
        }
        Location3D mineLocation = AnimationHelpers.getBlockFromAction(getSparkLocation(), getSparkFaceDirection(),
                animationType);
        if (world.isAirBlock(mineLocation.X, mineLocation.Y, mineLocation.Z)) {
            return FlameActionTargetResponse.fromJson("{\"did_pass\":true,\"did_break\":false,\"did_gather\":false}");
        }
        boolean did_gather = true;
        Block blockToMine = world.getBlock(mineLocation.X, mineLocation.Y, mineLocation.Z);
        ArrayList<ItemStack> items = blockToMine.getDrops(world, mineLocation.X, mineLocation.Y, mineLocation.Z,
                world.getBlockMetadata(mineLocation.X, mineLocation.Y, mineLocation.Z), 0);
        for (ItemStack itemStack : items) {
            // Try to add it to a slot
            for (int i = 0; i < m_inventory.length && itemStack.stackSize > 0; i++) {
                if (m_inventory[i] == null) {
                    m_inventory[i] = itemStack.copy();
                    itemStack.stackSize = 0;
                } else if (itemStack != null && m_inventory[i].getItem() == itemStack.getItem() &&
                        (!itemStack.getHasSubtypes() || m_inventory[i].getItemDamage() == itemStack.getItemDamage()) &&
                        ItemStack.areItemStackTagsEqual(itemStack, m_inventory[i])) {
                    // Stackable items, merge them
                    int finalCount = m_inventory[i].stackSize + itemStack.stackSize;
                    if (finalCount <= itemStack.getMaxStackSize()) {
                        // Put them all in
                        m_inventory[i].stackSize = finalCount;
                        itemStack.stackSize = 0;
                    } else {
                        // Put as many as we can
                        int overflow = finalCount - itemStack.getMaxStackSize();
                        itemStack.stackSize = overflow;
                        m_inventory[i].stackSize = itemStack.getMaxStackSize();
                    }
                }
            }
            // Drop the rest, if any
            if (itemStack.stackSize > 0) {
                world.spawnEntityInWorld(new EntityItem(world, getSparkLocation().X, getSparkLocation().Y,
                        getSparkLocation().Z, itemStack));
                did_gather = false;
            }
        }
        world.setBlockToAir(mineLocation.X, mineLocation.Y, mineLocation.Z);
        return FlameActionTargetResponse.fromJson("{\"did_pass\":true,\"did_break\":true,\"did_gather\":" +
                (did_gather ? "true" : "false") + "}");
    }

    @FlameActionPath("drop")
    public FlameActionTargetResponse dropAction(World world, JsonNode message) {
        Location3D mineLocation = AnimationHelpers.getBlockFromAction(getSparkLocation(), getSparkFaceDirection(),
                AnimationHelpers.AnimationTypes.Forward);
        for (int i = 0; i < m_inventory.length; i++) {
            if (m_inventory[i] != null) {
                world.spawnEntityInWorld(new EntityItem(world, getSparkLocation().X, getSparkLocation().Y,
                        getSparkLocation().Z, m_inventory[i]));
                m_inventory[i] = null;
            }
        }
        return FlameActionTargetResponse.fromJson("{\"did_pass\":true}");
    }

    @FlameActionPath("recall")
    public FlameActionTargetResponse recallAction(World world, JsonNode message) {
        TileEntityWoodenSpark tileEntityWoodenSpark = (TileEntityWoodenSpark) world.getTileEntity(
                getSparkLocation().X, getSparkLocation().Y, getSparkLocation().Z);
        // Suppress the custom block breaking effect
        tileEntityWoodenSpark.suppressDrop();
        world.setBlockToAir(getSparkLocation().X, getSparkLocation().Y, getSparkLocation().Z);
        // Suppress animation
        m_sparkFaceDir = AnimationHelpers.FaceDirections.North;
        m_sparkLocation = new Location3D(xCoord, yCoord, zCoord);
        animateServerAndSendToClients(world, AnimationHelpers.AnimationTypes.Idle);
        return FlameActionTargetResponse.fromJson("{\"did_pass\":true}");
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

    public TileEntityWoodenSpark getWoodenSpark() {
        TileEntity tileEntity = worldObj.getTileEntity(getSparkLocation().X, getSparkLocation().Y,
                getSparkLocation().Z);
        if (tileEntity != null && tileEntity instanceof TileEntityWoodenSpark) {
            return (TileEntityWoodenSpark) tileEntity;
        } else {
            return null;
        }
    }

    public Location3D getPadLocation() {
        return new Location3D(xCoord, yCoord, zCoord);
    }

    // ======   Helpers   ==============================================================================================
    private void animateServerAndSendToClients(World world, AnimationHelpers.AnimationTypes animationType) {
        m_sparkAnimation = animationType;
        m_sparkFaceDir = AnimationHelpers.getNewFaceDirByAnimation(m_sparkFaceDir, animationType);
        m_animationTimer = new CountdownTimer(ANIMATION_TIME);
        world.markBlockForUpdate(xCoord, yCoord, zCoord);
        world.markBlockForUpdate(getSparkLocation().X, getSparkLocation().Y, getSparkLocation().Z);
        super.markDirty();
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
        return i;
    }

    @Override
    public int getEnergyStored(ForgeDirection forgeDirection) {
        return 0;
    }

    @Override
    public int getMaxEnergyStored(ForgeDirection forgeDirection) {
        return 0;
    }

    @Override
    public boolean canConnectEnergy(ForgeDirection forgeDirection) {
        return true;
    }

}
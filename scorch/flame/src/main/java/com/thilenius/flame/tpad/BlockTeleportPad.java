package com.thilenius.flame.tpad;

import com.thilenius.flame.Flame;
import com.thilenius.flame.GlobalData;
import com.thilenius.flame.spark.TileEntityWoodenSpark;
import cpw.mods.fml.client.registry.ClientRegistry;
import cpw.mods.fml.common.registry.GameRegistry;
import net.minecraft.block.Block;
import net.minecraft.block.BlockContainer;
import net.minecraft.block.material.Material;
import net.minecraft.entity.item.EntityItem;
import net.minecraft.entity.player.EntityPlayer;
import net.minecraft.item.ItemStack;
import net.minecraft.nbt.NBTTagCompound;
import net.minecraft.tileentity.TileEntity;
import net.minecraft.world.World;

import java.util.Random;

public class BlockTeleportPad extends BlockContainer {

    public BlockTeleportPad() {
        super(Material.rock);
        setBlockName("blockTeleportPad");
        setHardness(20.0f);
        setResistance(10000000000.0f);
        setHarvestLevel("pickaxe", 0);
        setBlockTextureName("flame:TeleportPad");
    }

    @Override
    public boolean onBlockActivated(World world, int x, int y, int z,
                                    EntityPlayer player, int metadata, float what, float these, float are) {
        TileEntity tileEntity = world.getTileEntity(x, y, z);
        if (tileEntity == null || player.isSneaking()) {
            return false;
        }
        player.openGui(Flame.instance, 0, world, x, y, z);
        return true;
    }

    @Override
    public void onBlockAdded(World world, int x, int y, int z)
    {
        super.onBlockAdded(world, x, y, z);
    }

    @Override
    public int quantityDropped(int meta, int fortune, Random random) {
        return 0;
    }

    @Override
    public void breakBlock(World world, int x, int y, int z, Block block, int noIdeaWhatThisIs)
    {
        TileEntity tileEntity = world.getTileEntity(x, y, z);
        if (tileEntity instanceof TileEntityTeleportPad) {
            TileEntityTeleportPad tileEntityTeleportPad = (TileEntityTeleportPad) tileEntity;
            tileEntityTeleportPad.unregisterActionPaths();
            if (!world.isRemote) {
                world.spawnEntityInWorld(new EntityItem(world, x, y, z, new ItemStack(GlobalData.TeleportPadItem, 1)));
            }
            // Also break the spark
            TileEntity tileEntity2 = world.getTileEntity(
                    tileEntityTeleportPad.getSparkLocation().X,
                    tileEntityTeleportPad.getSparkLocation().Y,
                    tileEntityTeleportPad.getSparkLocation().Z);
            if (tileEntity2 != null && tileEntity2 instanceof TileEntityWoodenSpark) {
                TileEntityWoodenSpark tileEntityWoodenSpark = (TileEntityWoodenSpark) tileEntity2;
                world.setBlockToAir(tileEntityWoodenSpark.xCoord,
                        tileEntityWoodenSpark.yCoord,
                        tileEntityWoodenSpark.zCoord);
            }
        }
    }

    // =================================================================================================================
    // ====  TileEntity Support  =======================================================================================
    // =================================================================================================================
    @Override
    public TileEntity createNewTileEntity(World world, int p_149915_2_) {
        return new TileEntityTeleportPad();
    }

    @Override
    public boolean isOpaqueCube() {
        return false;
    }

    @Override
    public int getRenderType() {
        return -1;
    }

    @Override
    public boolean renderAsNormalBlock() {
        return false;
    }
}
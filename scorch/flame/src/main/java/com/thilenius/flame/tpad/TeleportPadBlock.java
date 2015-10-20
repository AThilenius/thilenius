package com.thilenius.flame.tpad;

import com.thilenius.flame.Flame;
import cpw.mods.fml.client.registry.ClientRegistry;
import cpw.mods.fml.common.registry.GameRegistry;
import net.minecraft.block.Block;
import net.minecraft.block.BlockContainer;
import net.minecraft.block.material.Material;
import net.minecraft.entity.player.EntityPlayer;
import net.minecraft.tileentity.TileEntity;
import net.minecraft.world.World;

public class TeleportPadBlock extends BlockContainer {

    public TeleportPadBlock() {
        super(Material.ground);
        GameRegistry.registerBlock(this, "teleportPadBlock");
        GameRegistry.registerTileEntity(TeleportPadTileEntity.class, "teleportPadTileEntity");
        ClientRegistry.bindTileEntitySpecialRenderer(TeleportPadTileEntity.class, new TeleportPadRenderer());
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
        TeleportPadTileEntity teleportPadTileEntity = (TeleportPadTileEntity) world.getTileEntity(x, y, z);
        teleportPadTileEntity.registerActionPaths();
        super.onBlockAdded(world, x, y, z);
    }

    @Override
    public void breakBlock(World world, int x, int y, int z, Block block, int noIdeaWhatThisIs)
    {
        TeleportPadTileEntity teleportPadTileEntity = (TeleportPadTileEntity) world.getTileEntity(x, y, z);
        teleportPadTileEntity.unregisterActionPaths();
        super.breakBlock(world, x, y, z, block, noIdeaWhatThisIs);
    }

    // =================================================================================================================
    // ====  TileEntity Support  =======================================================================================
    // =================================================================================================================
    @Override
    public TileEntity createNewTileEntity(World world, int p_149915_2_) {
        return new TeleportPadTileEntity();
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
package com.thilenius.flame;

import com.thilenius.flame.tpad.*;
import cpw.mods.fml.common.network.IGuiHandler;
import net.minecraft.entity.player.EntityPlayer;
import net.minecraft.tileentity.TileEntity;
import net.minecraft.world.World;

//public class GuiHandler implements IGuiHandler {
//
//    // Returns an object to the client that represents things like container state
//    @Override
//    public Object getServerGuiElement(int id, EntityPlayer player, World world, int x, int y, int z) {
//        TileEntity tileEntity = world.getTileEntity(x, y, z);
//        if(tileEntity instanceof WoodenSparkTileEntity){
//            WoodenSparkTileEntity spark = (WoodenSparkTileEntity) tileEntity;
//            TeleportPadTileEntity teleportPad = spark.getTeleportPad();
//            return new TeleportPadContainer(player.inventory, teleportPad);
//        }
//        return null;
//    }
//
//    @Override
//    public Object getClientGuiElement(int id, EntityPlayer player, World world, int x, int y, int z) {
//        TileEntity tileEntity = world.getTileEntity(x, y, z);
//        if(tileEntity instanceof TeleportPadTileEntity){
//            return new TeleportPadGui(tileEntity);
//        } else if (tileEntity instanceof WoodenSparkTileEntity) {
//            WoodenSparkTileEntity spark = (WoodenSparkTileEntity) tileEntity;
//            TeleportPadTileEntity teleportPad = spark.getTeleportPad();
//            return new WoodenSparkGui(player.inventory, teleportPad);
//        }
//        return null;
//    }
//}

public class GuiHandler implements IGuiHandler {
    //returns an instance of the Container you made earlier
    @Override
    public Object getServerGuiElement(int id, EntityPlayer player, World world,
                                      int x, int y, int z) {
        TileEntity tileEntity = world.getTileEntity(x, y, z);
        if(tileEntity instanceof WoodenSparkTileEntity) {
            WoodenSparkTileEntity woodenSparkTileEntity = (WoodenSparkTileEntity) tileEntity;
            TeleportPadTileEntity teleportPadTileEntity = woodenSparkTileEntity.getTeleportPad();
            return new WoodenSparkContainer(player.inventory, teleportPadTileEntity);
        } else if (tileEntity instanceof TeleportPadTileEntity) {
            // NA
        }
        return null;
    }

    //returns an instance of the Gui you made earlier
    @Override
    public Object getClientGuiElement(int id, EntityPlayer player, World world,
                                      int x, int y, int z) {
        TileEntity tileEntity = world.getTileEntity(x, y, z);
        if(tileEntity instanceof WoodenSparkTileEntity) {
            WoodenSparkTileEntity woodenSparkTileEntity = (WoodenSparkTileEntity) tileEntity;
            TeleportPadTileEntity teleportPadTileEntity = woodenSparkTileEntity.getTeleportPad();
            return new WoodenSparkGui(player.inventory, teleportPadTileEntity);
        } else if (tileEntity instanceof TeleportPadTileEntity) {
            return new TeleportPadGui((TeleportPadTileEntity)tileEntity);
        }
        return null;

    }
}
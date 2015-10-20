package com.thilenius.flame.tpad;

import com.thilenius.flame.GlobalData;
import cpw.mods.fml.common.registry.GameRegistry;
import net.minecraft.creativetab.CreativeTabs;
import net.minecraft.entity.player.EntityPlayer;
import net.minecraft.init.Blocks;
import net.minecraft.init.Items;
import net.minecraft.item.Item;
import net.minecraft.item.ItemStack;
import net.minecraft.world.World;

/**
 * Created by Alec on 10/17/15.
 */
public class TeleportPadItem extends Item {

    public TeleportPadItem() {
        maxStackSize = 1;
        setCreativeTab(CreativeTabs.tabMisc);
        setUnlocalizedName("teleportPad");
//        GameRegistry.addRecipe(new ItemStack(this), new Object[]{
//                "ABA",
//                "ACA",
//                "ADA",
//                'A', Blocks.planks, 'B', Blocks.redstone_lamp, 'C', GlobalData.SingularityCoreItem, 'D', Blocks.chest
//        });
        GameRegistry.addRecipe(new ItemStack(this), new Object[]{
                "AAA",
                "ABA",
                "AAA",
                'A', Blocks.cobblestone, 'B', GlobalData.SingularityCoreItem
        });
        this.setTextureName("flame:TeleportPad");
    }

    public boolean onItemUse(ItemStack itemStack, EntityPlayer entityPlayer, World world, int x, int y, int z, int size,
                             float px, float py, float pz) {
        if (!world.isRemote) {
            System.out.println("OnItemUse: " + itemStack.toString() + ":" + entityPlayer.getGameProfile().getName() +
                    ":" + x + ":" + y + ":" + z);
            world.setBlock(x, y, z, GlobalData.TeleportPadBlock);
            TeleportPadTileEntity teleportPadTileEntity = (TeleportPadTileEntity) world.getTileEntity(x, y, z);
            teleportPadTileEntity.setPlayerName(entityPlayer.getGameProfile().getName());
            entityPlayer.destroyCurrentEquippedItem();
            return true;
        } else {
            return false;
        }
    }

}

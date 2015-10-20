package com.thilenius.flame.tpad;

import cpw.mods.fml.common.registry.GameRegistry;
import net.minecraft.creativetab.CreativeTabs;
import net.minecraft.init.Blocks;
import net.minecraft.init.Items;
import net.minecraft.item.Item;
import net.minecraft.item.ItemStack;

/**
 * Created by Alec on 10/17/15.
 */
public class SingularityCoreItem extends Item {

    public SingularityCoreItem() {
        maxStackSize = 1;
        setCreativeTab(CreativeTabs.tabMisc);
        setUnlocalizedName("singularityCore");
        GameRegistry.addRecipe(new ItemStack(this), new Object[]{
                "BAB",
                "AAA",
                "BAB",
                'A', Items.redstone, 'B', Blocks.glass
        });
        this.setTextureName("flame:SingularityCore");
    }

}

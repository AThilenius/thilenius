package com.thilenius.flame.spark;

import com.thilenius.flame.tpad.TileEntityTeleportPad;
import net.minecraft.client.gui.inventory.GuiContainer;
import net.minecraft.entity.player.InventoryPlayer;
import net.minecraft.util.ResourceLocation;

public class GuiWoodenSpark extends GuiContainer {

    private ResourceLocation m_inventoryBackground = new ResourceLocation("textures/gui/container/dispenser.png");

    public GuiWoodenSpark(InventoryPlayer inventoryPlayer,
                          TileEntityTeleportPad tileEntity) {
        super(new ContainerWoodenSpark(inventoryPlayer, tileEntity));
    }

    @Override
    protected void drawGuiContainerForegroundLayer(int param1, int param2) {
        drawString(fontRendererObj, "Spark Inventory", 8, 6, 0xFFFFFF);
    }

    @Override
    protected void drawGuiContainerBackgroundLayer(float par1, int par2, int par3) {
        //int texture = mc.renderEngine.getTexture("/gui/trap.png");
        //GL11.glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
        //this.mc.renderEngine.bindTexture(texture);
        mc.renderEngine.bindTexture(m_inventoryBackground);
        int x = (width - xSize) / 2;
        int y = (height - ySize) / 2;
        this.drawTexturedModalRect(x, y, 0, 0, xSize, ySize);
    }

}
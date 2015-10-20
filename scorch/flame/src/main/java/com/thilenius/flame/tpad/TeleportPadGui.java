package com.thilenius.flame.tpad;

import com.thilenius.flame.Flame;
import com.thilenius.flame.utilities.types.Location3D;
import net.minecraft.client.gui.GuiButton;
import net.minecraft.client.gui.GuiScreen;
import net.minecraft.tileentity.TileEntity;
import net.minecraft.util.ResourceLocation;
import org.lwjgl.opengl.GL11;

import java.io.IOException;

public class TeleportPadGui extends GuiScreen {

    // Sizes
    private static int WINDOW_WIDTH = 256;
    private static int WINDOW_HEIGHT = 200;

    // GUI Components
    private GuiButton m_upButton;
    private GuiButton m_downButton;
    private GuiButton m_forwardButton;
    private GuiButton m_backwardButton;
    private GuiButton m_turnLeftButton;
    private GuiButton m_turnRightButton;

    private Location3D m_activeLocation;
    private TeleportPadTileEntity m_activeEntity;

    public TeleportPadGui(TileEntity tileEntity) {
        if (tileEntity instanceof WoodenSparkTileEntity) {
            WoodenSparkTileEntity spark = (WoodenSparkTileEntity) tileEntity;
            m_activeEntity = spark.getTeleportPad();
        } else if (tileEntity instanceof TeleportPadTileEntity) {
            m_activeEntity = (TeleportPadTileEntity) tileEntity;
        }
        m_activeLocation = new Location3D(tileEntity.xCoord, tileEntity.yCoord, tileEntity.zCoord);
    }

    @Override
    public void initGui() {
        this.buttonList.clear();
        // Base X, Y
        int posX = (width - WINDOW_WIDTH) / 2;
        int posY = (height - WINDOW_HEIGHT) / 2;
        // Constructors (ID, X, Y, Width, Height, Text)
        m_forwardButton = new GuiButton(0, posX + 50, posY + 90, 20, 20, "F");
        m_backwardButton = new GuiButton(1, posX + 50, posY + 120, 20, 20, "B");
        m_turnLeftButton = new GuiButton(2, posX + 20, posY + 120, 20, 20, "TL");
        m_turnRightButton = new GuiButton(3, posX + 80, posY + 120, 20, 20, "TR");
        // Add them
        buttonList.add(m_forwardButton);
        buttonList.add(m_backwardButton);
        buttonList.add(m_turnLeftButton);
        buttonList.add(m_turnRightButton);
    }

    @Override
    public void drawScreen(int x, int y, float f)
    {
        ResourceLocation texture = (new ResourceLocation("flame:textures/gui/Background.png"));
        GL11.glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
        this.mc.renderEngine.bindTexture(texture);
        // Draw GUI background image
        int guiX = (width - WINDOW_WIDTH) / 2;
        int guiY = (height - WINDOW_HEIGHT) / 2;
        this.drawTexturedModalRect(guiX, guiY, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        // Label
        drawString(fontRendererObj, "Frank Underwood", guiX + 20, guiY + 15, 0xFFFFFF);
        // Buttons and other controls
        super.drawScreen(x, y, f);
    }

    @Override
    public boolean doesGuiPauseGame() {
        return false;
    }

    @Override
    public void actionPerformed(GuiButton button) {
        try {
            switch (button.id) {
                case 0:
                    System.out.println("Forward");
                        m_activeEntity.moveAction(
                                Flame.Globals.JsonObjectMapper.readTree("{\"direction\":\"Forward\"}"));
                    break;
                case 1:
                    System.out.println("Backward");
                    m_activeEntity.moveAction(
                            Flame.Globals.JsonObjectMapper.readTree("{\"direction\":\"Backward\"}"));
                    break;
                case 2:
                    System.out.println("Turn Left");
                    break;
                case 3:
                    System.out.println("Turn Right");
                    break;
                default:
                    System.out.println("Failed to handle message for button with id " + button.id);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void keyTyped(char c, int i){
        super.keyTyped(c, i);
        // TODO: capture WASD, Space and Shift keys
    }

    @Override
    public void mouseClicked(int x, int y, int k){
        super.mouseClicked(x, y, k);
    }

}
package com.thilenius.flame.tpad;

import com.thilenius.flame.spark.ModelSparkSmall;
import com.thilenius.flame.utilities.AnimationHelpers;
import com.thilenius.flame.utilities.MathUtils;
import com.thilenius.flame.utilities.types.CountdownTimer;
import com.thilenius.flame.utilities.types.Location3D;
import com.thilenius.flame.utilities.types.LocationF3D;
import net.minecraft.client.renderer.tileentity.TileEntitySpecialRenderer;
import net.minecraft.entity.Entity;
import net.minecraft.tileentity.TileEntity;
import net.minecraft.util.ResourceLocation;
import net.minecraftforge.client.model.AdvancedModelLoader;
import net.minecraftforge.client.model.IModelCustom;
import org.lwjgl.opengl.GL11;

public class TeleportPadRenderer extends TileEntitySpecialRenderer {

    private IModelCustom m_teleportModel
            = AdvancedModelLoader.loadModel(new ResourceLocation("flame:models/TeleportPad.obj"));
    private ResourceLocation m_magicaColorsTexture
            = new ResourceLocation("flame:textures/blocks/MagicaVoxelColors.png");
    private ModelSparkSmall m_sparkModel = new ModelSparkSmall();

    // Can render both a TeleportPadTileEntity and a WoodenSparkTileEntity
    @Override
    public void renderTileEntityAt(TileEntity te, double x, double y, double z, float deltaTime) {
        LocationF3D location = new LocationF3D((float)x, (float)y, (float)z);
        TeleportPadTileEntity teleportPad = null;
        if (te instanceof WoodenSparkTileEntity) {
            WoodenSparkTileEntity spark = (WoodenSparkTileEntity) te;
            teleportPad = spark.getTeleportPad();
            // Render just the spark
            renderSpark(teleportPad, location);
        } else if (te instanceof TeleportPadTileEntity) {
            teleportPad = (TeleportPadTileEntity) te;
            // Render both the pad, and the spark on top of it if they are on the same location
            if (teleportPad.getPadLocation().equals(teleportPad.getSparkLocation())) {
                renderSpark(teleportPad, location);
            }
            renderTeleportPad(teleportPad, location);
        }
    }

    protected void renderTeleportPad(TeleportPadTileEntity teleportPad, LocationF3D location) {
        if (teleportPad == null) {
            return;
        }
        float scale = 1.0f;
        GL11.glPushMatrix();
        GL11.glTranslated(location.X + 0.5f, location.Y, location.Z + 0.5f);
        GL11.glScalef(scale, scale, scale);
        bindTexture(m_magicaColorsTexture);
        m_teleportModel.renderAll();
        GL11.glPopMatrix();
    }

    protected void renderSpark(TeleportPadTileEntity teleportPad, LocationF3D location) {
        if (teleportPad == null) {
            return;
        }
        CountdownTimer animationTimer = teleportPad.getAnimationTimer();
        AnimationHelpers.AnimationTypes animationType = teleportPad.getSparkAnimationType();
        AnimationHelpers.FaceDirections faceDirection = teleportPad.getSparkFaceDirection();
        float fractionTime = animationTimer != null ? animationTimer.getRemainingRatio() : -1.0f;
        // Compute Rotation
        float rotation = 0.0f;
        switch (faceDirection) {
            case North: rotation = 0.0f; break;
            case East: rotation = 90.0f; break;
            case South: rotation = 180.0f; break;
            case West: rotation = 270.0f; break;
        }
        if (fractionTime > 0.0f) {
            switch (animationType) {
                case TurnLeft: rotation += MathUtils.lerp(0.0f, 90.0f, fractionTime); break;
                case TurnRight: rotation += MathUtils.lerp(0.0f, -90.0f, fractionTime); break;
            }
        }
        // Compute Offset
        LocationF3D offset = new LocationF3D();
        if (fractionTime > 0.0f) {
            // Moving forward backward
            if (animationType == AnimationHelpers.AnimationTypes.Forward ||
                    animationType == AnimationHelpers.AnimationTypes.Backward) {
                LocationF3D facingDirection = AnimationHelpers.getRotationVector(faceDirection);
                // Reverse the rotation vector, because we are animating backward
                facingDirection = facingDirection.scale(-1.0f);
                if (animationType == AnimationHelpers.AnimationTypes.Backward) {
                    facingDirection = facingDirection.scale(-1.0f);
                }
                offset = facingDirection.scale(fractionTime);
            } else if (animationType == AnimationHelpers.AnimationTypes.Up ||
                    animationType == AnimationHelpers.AnimationTypes.Down) {
                float up = animationType == AnimationHelpers.AnimationTypes.Up ? -1.0f : 1.0f;
                offset = new LocationF3D(0.0f, up, 0.0f).scale(fractionTime);
            }
        }
        // Draw
        GL11.glPushMatrix();
        GL11.glTranslatef((float) location.X + 0.5f + offset.X,
                (float) location.Y + 0.35f + offset.Y,
                (float) location.Z + 0.6f + offset.Z);
        GL11.glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        GL11.glRotatef(rotation, 0.0f, 1.0f, 0.0f);
        ResourceLocation textures = (new ResourceLocation("flame:textures/model/Spark.png"));
        bindTexture(textures);
        m_sparkModel.render((Entity) null, 0.0F, 0.0F, -0.1F, 0.0F, 0.0F, 0.0625F);
        GL11.glPopMatrix();
    }

}
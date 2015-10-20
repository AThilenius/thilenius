package com.thilenius.flame;

import com.thilenius.flame.spark.TileEntityWoodenSpark;
import com.thilenius.flame.tpad.RendererTeleportPad;
import com.thilenius.flame.tpad.TileEntityTeleportPad;
import cpw.mods.fml.client.registry.ClientRegistry;

public class ClientProxy extends CommonProxy {
	@Override
	public void registerRenderers() {
        Flame.Globals.IsClient = true;
        ClientRegistry.bindTileEntitySpecialRenderer(TileEntityTeleportPad.class, new RendererTeleportPad());
        // Can use the dual-purpose teleport pad renderer for this
        ClientRegistry.bindTileEntitySpecialRenderer(TileEntityWoodenSpark.class, new RendererTeleportPad());
	}
}

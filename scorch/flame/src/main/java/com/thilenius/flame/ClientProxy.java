package com.thilenius.flame;

import cpw.mods.fml.client.registry.ClientRegistry;

public class ClientProxy extends CommonProxy {
	@Override
	public void registerRenderers() {
        Flame.Globals.IsClient = true;
	}
}

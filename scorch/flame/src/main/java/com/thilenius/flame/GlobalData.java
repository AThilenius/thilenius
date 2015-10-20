package com.thilenius.flame;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.thilenius.flame.entity.FlameEntityRegistry;
import com.thilenius.flame.rest.RestServer;
import net.minecraft.block.Block;
import net.minecraft.item.Item;

public class GlobalData {
    public ObjectMapper JsonObjectMapper = new ObjectMapper();
    public FlameEntityRegistry EntityRegistry = new FlameEntityRegistry();
    public Boolean IsClient = false;
    public RestServer RestServer = null;

    public static Item SingularityCoreItem;
    public static Item TeleportPadItem;
    public static Item WoodenSparkItem;

    public static Block TeleportPadBlock;
    public static Block WoodenSparkBlock;
}

package com.thilenius.flame.utilities.types;

/**
 * Created by Alec on 10/21/15.
 */
public class AnimationCycle {

    public float AnimationSpeed = 1.0f;
    public float AnimationScale = 1.0f;

    private long m_startTime = System.currentTimeMillis();

    public AnimationCycle() { }

    public AnimationCycle(float animationSpeed, float animationScale) {
        AnimationSpeed = animationSpeed;
        AnimationScale = animationScale;
    }

    public float getZeroToScaleCycle(float offsetConst) {
        long currentTime = System.currentTimeMillis();
        long deltaTime = currentTime - m_startTime;
        float deltaSeconds = deltaTime / 1000.0f;
        deltaSeconds *= AnimationSpeed;
        float deltaSecondsPlusOffset = deltaSeconds + offsetConst;
        float sinCycle = (float) Math.sin(deltaSecondsPlusOffset);
        float zeroOneCycle = (sinCycle + 1.0f) / 2.0f;
        float scaledCycle = zeroOneCycle * AnimationScale;
        return scaledCycle;
    }
}

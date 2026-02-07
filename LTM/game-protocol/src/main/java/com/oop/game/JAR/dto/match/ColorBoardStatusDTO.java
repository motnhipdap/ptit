package com.oop.game.JAR.dto.match;

import java.util.*;

import com.oop.game.JAR.dto.DTO;

/**
 * Trạng thái bảng màu (ẩn thông tin để client phải đoán)
 */
public class ColorBoardStatusDTO extends DTO {
    private List<String> visibleColors; // Màu nào đang hiện
    private boolean hasRecentSwap; // Có vừa hoán đổi màu không

    public ColorBoardStatusDTO(List<String> visibleColors, boolean hasRecentSwap) {
        this.visibleColors = visibleColors;
        this.hasRecentSwap = hasRecentSwap;

    }

    public List<String> getVisibleColors() {
        return visibleColors;
    }

    public void setVisibleColors(List<String> visibleColors) {
        this.visibleColors = visibleColors;
    }

    public boolean isHasRecentSwap() {
        return hasRecentSwap;
    }

    public void setHasRecentSwap(boolean hasRecentSwap) {
        this.hasRecentSwap = hasRecentSwap;
    }

}
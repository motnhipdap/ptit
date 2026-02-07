package server.utils;

import com.oop.game.JAR.dto.match.ColorBoardStatusDTO;
import com.oop.game.JAR.dto.match.MatchDTO;
import com.oop.game.JAR.dto.match.ThrowResultDTO;
import com.oop.game.JAR.dto.player.PlayerDTO;
import com.oop.game.JAR.dto.player.PlayerInMatchDTO;
import com.oop.game.JAR.dto.player.PlayerRankDTO;
import server.entities.Player;
import server.models.ColorBoardModel;
import server.models.MatchModel;
import server.models.PlayerModel;
import server.utils.GameEngine.ThrowResult;

import java.util.ArrayList;

/**
 * Model To DTO converter
 * Chuyển đổi từ các Model (server-side) sang DTO (client-side)
 */
public class MTD {

    /**
     * Convert PlayerModel sang PlayerInMatchDTO
     */
    public static PlayerInMatchDTO playerInMatch(PlayerModel model) {
        Player entity = model.getEntity();

        PlayerInMatchDTO dto = new PlayerInMatchDTO(
                entity.getId(),
                entity.getName(),
                entity.getElo(),
                model.getScore());

        dto.setPowerUps((ArrayList<String>) model.getPowerUpString());

        return dto;
    }

    /**
     * Convert MatchModel sang MatchDTO
     */
    public static MatchDTO match(MatchModel model) {
        PlayerInMatchDTO p1DTO = playerInMatch(model.getPlayer1());
        PlayerInMatchDTO p2DTO = playerInMatch(model.getPlayer2());
        ColorBoardStatusDTO boardDTO = model.getColorBoard().ToDTO(false);

        return new MatchDTO(
                model.getId(),
                p1DTO,
                p2DTO,
                boardDTO);
    }

    /**
     * Convert ThrowResult (từ GameEngine) sang ThrowResultDTO
     */
    public static ThrowResultDTO throwResult(ThrowResult result) {
        return new ThrowResultDTO(
                result.finalX,
                result.finalY,
                result.hitColor != null ? result.hitColor.name() : "NONE",
                result.scoreGained,
                result.finalScore,
                result.force,
                result.hasExtraTurn,
                result.canSwapColor);
    }

    /**
     * Convert ColorBoardModel sang ColorBoardStatusDTO
     */
    public static ColorBoardStatusDTO colorBoard(ColorBoardModel model, boolean hasRecentSwap) {
        return model.ToDTO(hasRecentSwap);
    }

    /**
     * Convert Player entity sang PlayerRankDTO (cho bảng xếp hạng)
     */
    public static PlayerRankDTO playerRank(Player player, int rank) {
        return new PlayerRankDTO(
                rank,
                player.getName(),
                player.getElo(),
                player.getTotalWin(),
                player.getTotalLoss());
    }

    /**
     * Convert Player entity sang PlayerDTO
     */
    public static PlayerDTO player(Player player, boolean isBusy) {
        return new PlayerDTO(
                player.getId(),
                player.getName(),
                player.getElo(),
                player.getTotalWin(),
                player.getTotalLoss(),
                player.getTotalMatch(),
                isBusy);
    }
}

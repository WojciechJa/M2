/*
 Navicat Premium Data Transfer

 Source Server         : VPS
 Source Server Type    : MariaDB
 Source Server Version : 101110 (10.11.10-MariaDB-log)
 Source Host           : 176.122.226.4:3306
 Source Schema         : srv2_player

 Target Server Type    : MariaDB
 Target Server Version : 101110 (10.11.10-MariaDB-log)
 File Encoding         : 65001

 Date: 18/02/2025 22:58:20
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for event_table
-- ----------------------------
DROP TABLE IF EXISTS `event_table`;
CREATE TABLE `event_table`  (
  `id` int(11) NOT NULL DEFAULT 0,
  `eventIndex` enum('BONUS_EVENT','DOUBLE_BOSS_LOOT_EVENT','DOUBLE_METIN_LOOT_EVENT','DOUBLE_MISSION_BOOK_EVENT','DUNGEON_COOLDOWN_EVENT','DUNGEON_CHEST_LOOT_EVENT','EMPIRE_WAR_EVENT','MOONLIGHT_EVENT','TOURNAMENT_EVENT','WHELL_OF_FORTUNE_EVENT','HALLOWEEN_EVENT','NPC_SEARCH_EVENT','EXP_EVENT','ITEM_DROP_EVENT','YANG_DROP_EVENT') CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT 'BONUS_EVENT',
  `startTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `endTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `empireFlag` int(11) NOT NULL DEFAULT 0,
  `channelFlag` int(11) NOT NULL DEFAULT 0,
  `value0` int(11) NOT NULL DEFAULT 0,
  `value1` int(11) NOT NULL DEFAULT 0,
  `value2` int(11) NOT NULL DEFAULT 0,
  `value3` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Fixed;

-- ----------------------------
-- Records of event_table
-- ----------------------------

SET FOREIGN_KEY_CHECKS = 1;

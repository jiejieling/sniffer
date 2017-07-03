-- MySQL dump 10.13  Distrib 5.1.72, for unknown-linux-gnu (x86_64)
--
-- Host: localhost    Database: sniffer_monitor
-- ------------------------------------------------------
-- Server version	5.1.72-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;


--
-- Current Database: `sniffer_monitor`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `sniffer_monitor` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `sniffer_monitor`;
--
-- Table structure for table `external`
--

DROP TABLE IF EXISTS `external`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `external` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `svr_id` int(11) DEFAULT NULL,
  `sip` varchar(16) DEFAULT NULL,
  `bytes` bigint(20) unsigned DEFAULT NULL,
  `packs` int(11) DEFAULT NULL,
  `fbytes` bigint(20) unsigned DEFAULT NULL,
  `fpacks` int(11) DEFAULT NULL,
  `rbytes` bigint(20) unsigned DEFAULT NULL,
  `rpacks` int(11) DEFAULT NULL,
  `tcp` int(11) DEFAULT NULL,
  `udp` int(11) DEFAULT NULL,
  `icmp` int(11) DEFAULT NULL,
  `other` int(11) DEFAULT NULL,
  `updatetime` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `updatetime` (`updatetime`),
  KEY `svr_id` (`svr_id`),
  KEY `sip_2` (`sip`,`svr_id`,`updatetime`)
) ENGINE=InnoDB AUTO_INCREMENT=86886290 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `internal`
--

DROP TABLE IF EXISTS `internal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `internal` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `svr_id` int(11) DEFAULT NULL,
  `sip` varchar(16) DEFAULT NULL,
  `bytes` bigint(20) unsigned DEFAULT NULL,
  `packs` int(11) DEFAULT NULL,
  `fbytes` bigint(20) unsigned DEFAULT NULL COMMENT '进流量',
  `fpacks` int(11) DEFAULT NULL COMMENT '进包数',
  `rbytes` bigint(20) unsigned DEFAULT NULL COMMENT '出流量',
  `rpacks` int(11) DEFAULT NULL COMMENT '出包数',
  `tcp` int(11) DEFAULT NULL,
  `udp` int(11) DEFAULT NULL,
  `icmp` int(11) DEFAULT NULL,
  `other` int(11) DEFAULT NULL,
  `updatetime` datetime DEFAULT NULL,
  `mth` date NOT NULL DEFAULT '0000-00-00',
  PRIMARY KEY (`id`),
  KEY `union_index` (`updatetime`,`sip`),
  KEY `ip_mth` (`mth`,`sip`),
  KEY `sip_2` (`sip`,`svr_id`,`updatetime`)
) ENGINE=InnoDB AUTO_INCREMENT=445215617 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `svr_info`
--

DROP TABLE IF EXISTS `svr_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `svr_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(16) DEFAULT NULL,
  `appid` int(11) DEFAULT NULL COMMENT '×Ê²úid',
  `idc` varchar(5) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `ip` (`ip`)
) ENGINE=InnoDB AUTO_INCREMENT=35 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping events for database 'sniffer_monitor'
--
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-07-29 10:10:44

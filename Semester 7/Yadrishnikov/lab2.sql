-- MariaDB dump 10.19  Distrib 10.5.15-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: lab2
-- ------------------------------------------------------
-- Server version	10.5.15-MariaDB-0+deb11u1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Заказчики`
--

DROP TABLE IF EXISTS `Заказчики`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Заказчики` (
  `КодЗаказчика` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `НазваниеФирмы` char(30) NOT NULL,
  `Руководитель` char(30) NOT NULL,
  `НомерСчета` char(30) NOT NULL,
  `Телефон` char(30) NOT NULL,
  `Почта` char(30) NOT NULL,
  PRIMARY KEY (`КодЗаказчика`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Заказчики`
--

LOCK TABLES `Заказчики` WRITE;
/*!40000 ALTER TABLE `Заказчики` DISABLE KEYS */;
INSERT INTO `Заказчики` VALUES (1,'ОАО Вирост','Албеков А.А.','1231252131','+7-913-423-23-30','fucsia@os.ru'),(2,'ЗАО ВимПласт','Шушарин Б.К.','1231212312','+7-913-523-33-70','kukuruzka@gmilo.ru'),(3,'ООО ХилПик','Болатаев А.В.','1741256131','+7-364-873-23-30','hunkos@jdm.ru'),(4,'ТЦ Олимпик','Игрушкин А.Ф.','8131253134','+7-913-544-12-73','toyjoy@game.com'),(5,'ИП Доставляйка','Оптимистов П.В.','1231213131','+7-913-463-43-87','lobostr@mailco.ru'),(6,'ОАО Хорошка','Нифтянов А.Р.','7231255821','+7-953-394-30-12','bestmail@mail.com'),(7,'ЗАО Дороги','Изподмятев А.А.','5631222131','+7-913-573-83-30','lenitun@fab.ru');
/*!40000 ALTER TABLE `Заказчики` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Заказы`
--

DROP TABLE IF EXISTS `Заказы`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Заказы` (
  `КодЗаказа` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `КодЗаказчика` int(10) unsigned NOT NULL,
  `КодУсилителя` int(10) unsigned NOT NULL,
  `Количество` int(10) unsigned NOT NULL,
  `ДатаЗаказа` date NOT NULL,
  PRIMARY KEY (`КодЗаказа`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Заказы`
--

LOCK TABLES `Заказы` WRITE;
/*!40000 ALTER TABLE `Заказы` DISABLE KEYS */;
INSERT INTO `Заказы` VALUES (1,1,2,10,'2022-10-22'),(2,3,5,100,'2022-10-24'),(3,7,2,250,'2022-10-25'),(4,2,1,100,'2022-10-26'),(5,4,3,1000,'2022-10-26'),(6,6,8,500,'2022-10-28'),(7,5,4,650,'2022-10-29'),(8,3,2,100,'2022-10-30'),(9,7,1,1000,'2022-11-04'),(10,6,8,500,'2022-11-05');
/*!40000 ALTER TABLE `Заказы` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Заказы2`
--

DROP TABLE IF EXISTS `Заказы2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Заказы2` (
  `КодЗаказа` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `КодЗаказчика` int(10) unsigned NOT NULL,
  `КодУсилителя` int(10) unsigned NOT NULL,
  `Количество` int(10) unsigned NOT NULL,
  `ДатаЗаказа` date NOT NULL,
  PRIMARY KEY (`КодЗаказа`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Заказы2`
--

LOCK TABLES `Заказы2` WRITE;
/*!40000 ALTER TABLE `Заказы2` DISABLE KEYS */;
INSERT INTO `Заказы2` VALUES (1,3,4,100,'2022-11-07'),(2,4,3,500,'2022-11-08'),(3,6,1,500,'2022-11-09');
/*!40000 ALTER TABLE `Заказы2` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ОперационныеУсилители`
--

DROP TABLE IF EXISTS `ОперационныеУсилители`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ОперационныеУсилители` (
  `КодУсилителя` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Наименование` char(30) NOT NULL,
  `Напряжение` int(10) unsigned NOT NULL,
  `ВыходнойТок` int(10) unsigned NOT NULL,
  `Цена` int(10) unsigned NOT NULL,
  PRIMARY KEY (`КодУсилителя`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ОперационныеУсилители`
--

LOCK TABLES `ОперационныеУсилители` WRITE;
/*!40000 ALTER TABLE `ОперационныеУсилители` DISABLE KEYS */;
INSERT INTO `ОперационныеУсилители` VALUES (1,'К140УД10',15,250,123),(2,'К140УД11',15,500,200),(3,'К140УД12',10,50,100),(4,'К140УД14',12,5,167),(5,'К140УД1Б',15,8000,111),(6,'К140УД7',12,400,120),(7,'К140УД8',8,2,213),(8,'К140УД9',12,350,114);
/*!40000 ALTER TABLE `ОперационныеУсилители` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2022-12-25 18:10:56

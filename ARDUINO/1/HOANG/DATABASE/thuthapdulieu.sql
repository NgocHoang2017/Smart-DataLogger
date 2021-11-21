-- phpMyAdmin SQL Dump
-- version 4.8.4
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Mar 05, 2019 at 05:47 PM
-- Server version: 10.1.37-MariaDB
-- PHP Version: 7.3.0

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `thuthapdulieu`
--

-- --------------------------------------------------------

--
-- Table structure for table `data`
--

CREATE TABLE `data` (
  `ID` int(10) NOT NULL,
  `date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `nhiet_do` varchar(255) NOT NULL,
  `do_am` varchar(255) NOT NULL,
  `soild` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `data`
--

INSERT INTO `data` (`ID`, `date`, `nhiet_do`, `do_am`, `soild`) VALUES
(1, '2019-03-01 23:31:36', '20', '30', '40'),
(2, '2019-03-01 23:32:11', '30', '40', '50'),
(3, '2019-03-01 23:33:57', '50', '60', '70'),
(6, '2019-03-05 19:39:03', '70', '80', '90');

-- --------------------------------------------------------

--
-- Table structure for table `tbl_messages`
--

CREATE TABLE `tbl_messages` (
  `messageID` int(255) NOT NULL,
  `DateTime_created` timestamp(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6),
  `topic` varchar(255) NOT NULL,
  `nhiet_do` varchar(255) NOT NULL,
  `do_am` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `tbl_messages`
--

INSERT INTO `tbl_messages` (`messageID`, `DateTime_created`, `topic`, `nhiet_do`, `do_am`) VALUES
(1, '0000-00-00 00:00:00.000000', 'Slave1', '30', '60'),
(2, '0000-00-00 00:00:00.000000', 'Slave1', '50', '60'),
(1, '0000-00-00 00:00:00.000000', 'Slave1', '30', '60'),
(2, '0000-00-00 00:00:00.000000', 'Slave1', '50', '60'),
(5, '2019-01-21 05:47:39.000000', '5', '31', '13'),
(5, '2019-01-21 05:47:42.000000', '5', '31', '13'),
(0, '2019-01-22 17:28:48.731924', '', '20', '10'),
(0, '2019-01-22 17:33:11.569501', '', '20', '10');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `data`
--
ALTER TABLE `data`
  ADD PRIMARY KEY (`ID`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;

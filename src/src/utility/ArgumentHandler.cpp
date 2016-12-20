/*-------------------------------------------------------------------------------
This file is part of Ranger.

Ranger is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ranger is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ranger. If not, see <http://www.gnu.org/licenses/>.

Written by:

Marvin N. Wright
Institut für Medizinische Biometrie und Statistik
Universität zu Lübeck
Ratzeburger Allee 160
23562 Lübeck

http://www.imbs-luebeck.de
wright@imbs.uni-luebeck.de
#-------------------------------------------------------------------------------*/

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "ArgumentHandler.h"
#include "version.h"
#include "utility.h"

ArgumentHandler::ArgumentHandler(int argc, char **argv) :
    caseweights(""), depvarname(""), fraction(1), savemem(false), predict(""),
    splitweights(""), nthreads(DEFAULT_NUM_THREADS), file(""),
    targetpartitionsize(0), mtry(0),
    quantiles(new std::vector<double>()), statusvarname(""), instrumentvarname(""),
    ntree(DEFAULT_NUM_TREE), replace(true), verbose(false), write(false), treetype(TREE_QUANTILE), seed(0) {
  this->argc = argc;
  this->argv = argv;
}

ArgumentHandler::~ArgumentHandler() {
}

int ArgumentHandler::processArguments() {

  // short options
  char const *short_options = "A:C:D:F:HM:NP:S:U:XZa:b:c:f:hil::m:o:pr:s:t:uvwy:z:";

  // long options: longname, no/optional/required argument?, flag(not used!), shortname
    const struct option long_options[] = {

      { "alwayssplitvars",      required_argument,  0, 'A'},
      { "caseweights",          required_argument,  0, 'C'},
      { "depvarname",           required_argument,  0, 'D'},
      { "fraction",             required_argument,  0, 'F'},
      { "savemem",              no_argument,        0, 'N'},
      { "predict",              required_argument,  0, 'P'},
      { "splitweights",         required_argument,  0, 'S'},
      { "nthreads",             required_argument,  0, 'U'},
      { "predall",              no_argument,        0, 'X'},
      { "version",              no_argument,        0, 'Z'},

      { "file",                 required_argument,  0, 'f'},
      { "help",                 no_argument,        0, 'h'},
      { "targetpartitionsize",  required_argument,  0, 'l'},
      { "mtry",                 required_argument,  0, 'm'},
      { "quantiles",            required_argument,  0, 'q'},
      { "splitrule",            required_argument,  0, 'r'},
      { "statusvarname",        required_argument,  0, 's'},
      { "instrumentvarname",    required_argument,  0, 'i'},
      { "ntree",                required_argument,  0, 't'},
      { "noreplace",            no_argument,        0, 'u'},
      { "verbose",              no_argument,        0, 'v'},
      { "write",                no_argument,        0, 'w'},
      { "treetype",             required_argument,  0, 'y'},
      { "seed",                 required_argument,  0, 'z'},

      { 0, 0, 0, 0}
    };

  while (1) {
    int option_index = 0;
    int c = getopt_long(argc, argv, short_options, long_options, &option_index);

    // stop if no options left
    if (c == -1) {
      break;
    }

    switch (c) {

    // upper case options
    case 'A':
      splitString(alwayssplitvars, optarg, ',');
      break;

    case 'C':
      caseweights = optarg;
      break;

    case 'D':
      depvarname = optarg;
      break;

    case 'F':
      try {
        fraction = std::stod(optarg);
        if (fraction > 1 || fraction <= 0) {
          throw std::runtime_error("");
        }
      } catch (...) {
        throw std::runtime_error(
            "Illegal argument for option 'fraction'. Please give a value in (0,1]. See '--help' for details.");
      }
      break;

    case 'N':
      savemem = true;
      break;

    case 'P':
      predict = optarg;
      break;

    case 'S':
      splitweights = optarg;
      break;

    case 'U':
      try {
        int temp = std::stoi(optarg);
        if (temp < 1) {
          throw std::runtime_error("");
        } else {
          nthreads = temp;
        }
      } catch (...) {
        throw std::runtime_error(
            "Illegal argument for option 'nthreads'. Please give a positive integer. See '--help' for details.");
      }
      break;

    case 'Z':
      displayVersion();
      return -1;
      break;

    case 'f':
      file = optarg;
      break;

    case 'h':
      displayHelp();
      return -1;
      break;

    case 'l':
      try {
        int temp = std::stoi(optarg);
        if (temp < 1) {
          throw std::runtime_error("");
        } else {
          targetpartitionsize = temp;
        }
      } catch (...) {
        throw std::runtime_error(
            "Illegal argument for option 'targetpartitionsize'. Please give a positive integer. See '--help' for details.");
      }
      break;

    case 'm':
      try {
        int temp = std::stoi(optarg);
        if (temp < 1) {
          throw std::runtime_error("");
        } else {
          mtry = temp;
        }
      } catch (...) {
        throw std::runtime_error(
            "Illegal argument for option 'mtry'. Please give a positive integer. See '--help' for details.");
      }
      break;

    case 'q': {
      std::vector<std::string> split_args;
      splitString(split_args, optarg, ',');

      for (auto &arg : split_args) {
        double quantile = std::stod(arg);
        if (quantile >= 1 || quantile <= 0) {
          throw std::runtime_error("All quantiles must lie in the range (0, 1).");
        }
        quantiles->push_back(quantile);
      }
      break;
    }
    case 's':
      statusvarname = optarg;
      break;

    case 'i':
      instrumentvarname = optarg;
      break;

    case 't':
      try {
        int temp = std::stoi(optarg);
        if (temp < 1) {
          throw std::runtime_error("");
        } else {
          ntree = temp;
        }
      } catch (...) {
        throw std::runtime_error(
            "Illegal argument for option 'ntree'. Please give a positive integer. See '--help' for details.");
      }
      break;

    case 'u':
      replace = false;
      break;

    case 'v':
      verbose = true;
      break;

    case 'w':
      write = true;
      break;

    case 'y':
      try {
        switch (std::stoi(optarg)) {
        case 11:
          treetype = TREE_QUANTILE;
          break;
        case 15:
          treetype = TREE_INSTRUMENTAL;
          break;
        default:
          throw std::runtime_error("");
          break;
        }
      } catch (...) {
        throw std::runtime_error(
            "Illegal argument for option 'treetype'. Please give a positive integer. See '--help' for details.");
      }
      break;

    case 'z':
      try {
        int temp = std::stoi(optarg);
        if (temp < 0) {
          throw std::runtime_error("");
        } else {
          seed = temp;
        }
      } catch (...) {
        throw std::runtime_error(
            "Illegal argument for option 'seed'. Please give a positive integer. See '--help' for details.");
      }
      break;

    default:
      break;

    }
  }

  // print all other parameters
  while (optind < argc) {
    std::cout << "Other parameter, not processed: " << argv[optind++] << std::endl;
  }

  return 0;
}

void ArgumentHandler::checkArguments() {

  // required arguments
  if (file.empty()) {
    throw std::runtime_error("Please specify an input filename with '--file'. See '--help' for details.");
  }
  if (predict.empty() && depvarname.empty()) {
    throw std::runtime_error("Please specify a dependent variable name with '--depvarname'. See '--help' for details.");
  }

  if (treetype == TREE_INSTRUMENTAL && instrumentvarname.empty()) {
    throw std::runtime_error("When using instrumental trees, the instrument variable must be specified through"
                                 "--instrumentvarname. See '--help' for details.");
  }

  if (treetype == TREE_INSTRUMENTAL && statusvarname.empty()) {
    throw std::runtime_error("When using instrumental trees, the treatment variable must be specified through"
                                 "--statusvarname. See '--help' for details.");
  }

  if (treetype == TREE_INSTRUMENTAL && instrumentvarname.empty()) {
    throw std::runtime_error("Option '--instrumentvarname' only applicable for instrumental forests. See '--help' for details.");
  }

  if (!alwayssplitvars.empty() && !splitweights.empty()) {
    throw std::runtime_error("Please use only one option of splitweights and alwayssplitvars.");
  }
}

void ArgumentHandler::displayHelp() {
  std::cout << "Usage: " << std::endl;
  std::cout << "    " << argv[0] << " [options]" << std::endl;
  std::cout << std::endl;

  std::cout << "Options:" << std::endl;
  std::cout << "    " << "--help                        Print this help." << std::endl;
  std::cout << "    " << "--version                     Print version and citation information." << std::endl;
  std::cout << "    " << "--verbose                     Turn on verbose mode." << std::endl;
  std::cout << "    " << "--file FILE                   Filename of input data. Only numerical values are supported." << std::endl;
  std::cout << "    " << "--treetype TYPE               Set tree type to:" << std::endl;
  std::cout << "    " << "                              TYPE = 1: Classification." << std::endl;
  std::cout << "    " << "                              TYPE = 3: Regression." << std::endl;
  std::cout << "    " << "                              TYPE = 11: Quantile." << std::endl;
  std::cout << "    " << "                              TYPE = 15: Instrumental." << std::endl;
  std::cout << "    " << "                              (Default: 1)" << std::endl;
  std::cout << "    " << "--quantiles                   The quantiles to predict when running a quantile forest (--treetype 11)." << std::endl;
  std::cout << "    " << "                              Note that all quantiles must lie in the range (0, 1)." << std::endl;
  std::cout << "    " << "--depvarname NAME             Name of dependent variable. For survival trees this is the time variable." << std::endl;
  std::cout << "    " << "--statusvarname NAME          Name of status variable, only applicable for survival and instrumental trees." << std::endl;
  std::cout << "    " << "                              Coding is 1 for event and 0 for censored." << std::endl;
  std::cout << "    " << "--instrumentvarname NAME      Name of instrument variable, only applicable for instrumental trees." << std::endl;
  std::cout << "    " << "--ntree N                     Set number of trees to N." << std::endl;
  std::cout << "    " << "                              (Default: 500)" << std::endl;
  std::cout << "    " << "--mtry N                      Number of variables to possibly split at in each node." << std::endl;
  std::cout << "    " << "                              (Default: sqrt(p) for Classification and Survival, p/3 for Regression. " << std::endl;
  std::cout << "    " << "                               p = number of independent variables)" << std::endl;
  std::cout << "    " << "--targetpartitionsize N       Set minimal node size to N." << std::endl;
  std::cout << "    " << "                              For Classification and Regression growing is stopped if a node reaches a size smaller than N." << std::endl;
  std::cout << "    " << "                              For Survival growing is stopped if one child would reach a size smaller than N." << std::endl;
  std::cout << "    " << "                              This means nodes with size smaller N can occur for Classification and Regression." << std::endl;
  std::cout << "    " << "                              (Default: 1 for Classification, 5 for Regression, and 3 for Survival)" << std::endl;
  std::cout << "    " << "--catvars V1,V2,..            Comma separated list of names of (unordered) categorical variables. " << std::endl;
  std::cout << "    " << "                              Categorical variables must contain only positive integer values." << std::endl;
  std::cout << "    " << "--write                       Save forest to file." << std::endl;
  std::cout << "    " << "--predict FILE                Load forest from FILE and predict with new data." << std::endl;
  std::cout << "    " << "--predall                     Return a matrix with individual predictions for each tree instead of aggregated " << std::endl;
  std::cout << "    " << "                              predictions for all trees (classification and regression only)." << std::endl;
  std::cout << "    " << "--impmeasure TYPE             Set importance mode to:" << std::endl;
  std::cout << "    " << "                              TYPE = 0: none." << std::endl;
  std::cout << "    " << "                              TYPE = 1: Node impurity: Gini for Classification, variance for Regression." << std::endl;
  std::cout << "    " << "                              TYPE = 2: Permutation importance, scaled by standard errors." << std::endl;
  std::cout << "    " << "                              TYPE = 3: Permutation importance, no scaling." << std::endl;
  std::cout << "    " << "                              (Default: 0)" << std::endl;
  std::cout << "    " << "--noreplace                   Sample without replacement." << std::endl;
  std::cout << "    " << "--fraction X                  Fraction of observations to sample. Default is 1 for sampling with replacement " << std::endl;
  std::cout << "    " << "                              and 0.632 for sampling without replacement." << std::endl;
  std::cout << "    " << "--caseweights FILE            Filename of case weights file." << std::endl;
  std::cout << "    " << "--splitweights FILE           Filename of split select weights file." << std::endl;
  std::cout << "    " << "--alwayssplitvars V1,V2,..    Comma separated list of variable names to be always considered for splitting." << std::endl;
  std::cout << "    " << "--nthreads N                  Set number of parallel threads to N." << std::endl;
  std::cout << "    " << "                              (Default: Number of CPUs available)" << std::endl;
  std::cout << "    " << "--seed SEED                   Set random seed to SEED." << std::endl;
  std::cout << "    " << "                              (Default: No seed)" << std::endl;
  std::cout << "    " << "--memmode MODE                Set memory mode to:" << std::endl;
  std::cout << "    " << "                              MODE = 0: double." << std::endl;
  std::cout << "    " << "                              MODE = 1: float." << std::endl;
  std::cout << "    " << "                              MODE = 2: char." << std::endl;
  std::cout << "    " << "                              (Default: 0)" << std::endl;
  std::cout << "    " << "--savemem                     Use memory saving (but slower) splitting mode." << std::endl;
  std::cout << std::endl;

  std::cout << "See README file for details and examples." << std::endl;
}

// TODO: Change citation info
void ArgumentHandler::displayVersion() {
  std::cout << "Ranger version: " << RANGER_VERSION << std::endl;
  std::cout << std::endl;
  std::cout << "Please cite Ranger: " << std::endl;
  std::cout << "Wright, M. N. & Ziegler, A. (2016). ranger: A Fast Implementation of Random Forests for High Dimensional Data in C++ and R. Journal of Statistical Software, in press." << std::endl;
  std::cout << std::endl;
  std::cout << "BibTeX:" << std::endl;
  std::cout << "@Article{," << std::endl;
  std::cout << "    title = {ranger: {{A}} fast implementation of random forests for high dimensional data in {{C}}++ and {{R}}}," << std::endl;
  std::cout << "    author = {Wright, Marvin N. and Ziegler, Andreas}," << std::endl;
  std::cout << "    journal = {Journal of Statistical Software}," << std::endl;
  std::cout << "    year = {2016}," << std::endl;
  std::cout << "}" << std::endl;
}
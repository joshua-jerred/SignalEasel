/**
 * =*========SignalEasel========*=
 * A friendly library for signal modulation and demodulation.
 * https://github.com/joshua-jerred/Giraffe
 * https://joshuajer.red/signal-easel
 * =*===========================*=
 *
 * @file   aprs_telemetry.hpp
 * @date   2024-08-16
 *
 * =*=======================*=
 * @copyright  2024 Joshua Jerred
 * @license    GNU GPLv3
 */

#pragma once

#include <regex>

#include <SignalEasel/aprs.hpp>
#include <SignalEasel/exception.hpp>

namespace signal_easel::aprs {

/// @brief A class to represent the 5 analog parameters in APRS telemetry
/// packets.
class AnalogParameter {
public:
  enum class Id : uint8_t { A1 = 0, A2 = 1, A3 = 2, A4 = 3, A5 = 4, UNKNOWN };

  /// @brief Default constructor. Sets the ID to UNKNOWN.
  AnalogParameter() = default;

  /// @brief Constructor that sets the ID of the analog parameter.
  /// @param id - The ID of the analog parameter, not UNKNOWN.
  AnalogParameter(Id id) : id_(id) {
    if (id == Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "Invalid analog parameter ID set.");
    }
  }

  /// @brief Destructor.
  ~AnalogParameter() = default;

  /// @brief Set the ID of the analog parameter.
  /// @param id - The ID of the analog parameter, not UNKNOWN.
  /// @throws signal_easel::Exception if the ID is UNKNOWN.
  void setId(Id id) {
    if (id == Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "Invalid analog parameter ID set.");
    }
    id_ = id;

    if (id_ == Id::A1) {
      // Weird APRS spec requirement requires a non-empty name and unit for A1
      if (name_.empty()) {

        name_ = "x";
      }
      if (unit_.empty()) {
        unit_ = "x";
      }
    }
  }

  /// @brief Get the ID of the analog parameter.
  /// @return The ID of the analog parameter.
  Id getId() const { return id_; }

  /// @brief Set the raw value of the analog parameter.
  /// @param raw_value - The raw value of the analog parameter.
  void setRawValue(uint8_t raw_value) { raw_value_ = raw_value; }

  /// @brief Get the raw value of the analog parameter.
  /// @return The raw value of the analog parameter.
  uint8_t getRawValue() const { return raw_value_; }

  /// @brief Set the name of the analog parameter.
  /// @param name - The name of the analog parameter.
  /// @throws signal_easel::Exception if name for a given ID is invalid
  /// (length).
  /// @note Does not validate characters.
  void setName(const std::string &name) {
    if (id_ == Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "Analog parameter ID not set.");
    }

    if (!validateAnalogDescriptor(name)) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "Invalid name length for analog parameter.");
    }

    name_ = name;
  }

  /// @brief Get the name of the analog parameter.
  /// @return The name of the analog parameter.
  std::string getName() const { return name_; }

  /// @brief Set the unit of the analog parameter.
  /// @param unit_or_label - The unit of the analog parameter.
  void setUnit(const std::string &unit) {
    if (id_ == Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "Analog parameter ID not set.");
    }

    if (!validateAnalogDescriptor(unit)) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "Invalid unit/label length for analog parameter.");
    }

    unit_ = unit;
  }

  /// @brief Get the unit of the analog parameter.
  /// @return The unit of the analog parameter.
  std::string getUnit() const { return unit_; }

  /// @brief Set the A coefficient for the analog parameter.
  /// @param coefficient - The A coefficient for the analog parameter.
  void setACoefficient(const std::string &coefficient) {
    if (!validateCoefficient(coefficient)) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "Invalid A coefficient for analog parameter.");
    }

    coefficient_a_ = coefficient;
  }
  /// @brief Set the B coefficient for the analog parameter.
  /// @param coefficient - The B coefficient for the analog parameter.
  void setBCoefficient(const std::string &coefficient) {
    if (!validateCoefficient(coefficient)) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "Invalid B coefficient for analog parameter.");
    }
    coefficient_b_ = coefficient;
  }
  /// @brief Set the C coefficient for the analog parameter.
  /// @param coefficient - The C coefficient for the analog parameter.
  void setCCoefficient(const std::string &coefficient) {
    if (!validateCoefficient(coefficient)) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "Invalid C coefficient for analog parameter.");
    }
    coefficient_c_ = coefficient;
  }
  /// @brief Set all coefficients for the analog parameter.
  /// @param a - The A coefficient for the analog parameter.
  /// @param b - The B coefficient for the analog parameter.
  /// @param c - The C coefficient for the analog parameter.
  void setCoefficients(const std::string &a, const std::string &b,
                       const std::string &c) {
    setACoefficient(a);
    setBCoefficient(b);
    setCCoefficient(c);
  }

  /// @brief Get the A coefficient for the analog parameter.
  /// @return The A coefficient for the analog parameter.
  std::string getACoefficient() const { return coefficient_a_; }
  /// @brief Get the B coefficient for the analog parameter.
  /// @return The B coefficient for the analog parameter.
  std::string getBCoefficient() const { return coefficient_b_; }
  /// @brief Get the C coefficient for the analog parameter.
  /// @return The C coefficient for the analog parameter.
  std::string getCCoefficient() const { return coefficient_c_; }

  /// @brief Simple validation of a coefficients for analog parameters.
  /// @param value - The coefficient to validate as a string.
  /// @return \c true if the value is valid, \c false if not.
  static bool validateCoefficient(const std::string &value) {
    // Coefficients must be between 1 and 9 characters long and may contain
    // characters 0-9 in any position, `-` at the beginning, and a single `.` in
    // any position.

    // Checks not done by regex first
    if (value.size() > 9 || value.empty()) { // Max length of 9
      return false;
    }

    if (value.size() == 1 && (value == "." || value == "-")) {
      return false;
    }

    if (value.size() == 2 && value == "-.") {
      return false;
    }

    const std::regex coefficient_regex("^-?\\d*\\.?\\d*$");

    return std::regex_match(value, coefficient_regex);
  }

private:
  /// @brief Validate the name, unit, or label of an analog parameter.
  /// @param value - The name, unit, or label of the analog parameter.
  /// @return \c true if the value is valid, \c false if not.
  bool validateAnalogDescriptor(const std::string &value) const {
    // In the spec, A1 *does not include a comma in the length*, but must be
    // of at least length 1. All others may have a length of 1.
    const size_t MIN_NAME_LENGTH_A1 = 1;
    const size_t MAX_NAME_LENGTH_A1 = 7;
    const size_t MAX_NAME_LENGTH_A2 = 6;
    const size_t MAX_NAME_LENGTH_A3_A4 = 5;
    const size_t MAX_NAME_LENGTH_A5 = 4;

    switch (id_) {
    case Id::A1:
      return value.size() >= MIN_NAME_LENGTH_A1 &&
             value.size() <= MAX_NAME_LENGTH_A1;
    case Id::A2:
      return value.size() <= MAX_NAME_LENGTH_A2;
    case Id::A3:
    case Id::A4:
      return value.size() <= MAX_NAME_LENGTH_A3_A4;
    case Id::A5:
      return value.size() <= MAX_NAME_LENGTH_A5;
      break;
    default:
      return false;
    };
  }

  /// @brief The ID of the analog parameter (A1 - A5)
  Id id_{Id::UNKNOWN};

  /// @brief The raw value of the analog parameter. 0 - 255
  uint8_t raw_value_{0};

  std::string name_{};
  std::string unit_{};

  /// @brief The a coefficient in the formula y = ax^2 + bx + c
  std::string coefficient_a_{"0"};
  /// @brief The b coefficient in the formula y = ax^2 + bx + c
  std::string coefficient_b_{"1"};
  /// @brief The c coefficient in the formula y = ax^2 + bx + c
  std::string coefficient_c_{"0"};
};

/// @brief A class to represent the 5 analog parameters in APRS telemetry
/// packets.
class DigitalParameter {
public:
  enum class Id : uint8_t {
    B1 = 0,
    B2 = 1,
    B3 = 2,
    B4 = 3,
    B5 = 4,
    B6 = 5,
    B7 = 6,
    B8 = 7,
    UNKNOWN
  };

  /// @brief Default constructor. Sets the ID to UNKNOWN.
  DigitalParameter() = default;

  /// @brief Constructor that sets the ID of the digital parameter.
  /// @param id - The ID of the digital parameter, not UNKNOWN.
  DigitalParameter(Id id) : id_(id) {
    if (id == Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "Invalid digital parameter ID set.");
    }

    id_ = id;
  }

  /// @brief Destructor.
  ~DigitalParameter() = default;

  /// @brief Set the ID of the digital parameter.
  /// @param id - The ID of the digital parameter, not UNKNOWN.
  /// @throws signal_easel::Exception if the ID is UNKNOWN.
  void setId(Id id) {
    if (id == Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "Invalid digital parameter ID set.");
    }

    id_ = id;
  }

  /// @brief Get the ID of the digital parameter.
  /// @return The ID of the digital parameter.
  Id getId() const { return id_; }

  /// @brief Set the value of the digital parameter.
  /// @param value - The value of the digital parameter.
  void setValue(bool value) { value_ = value; }

  /// @brief Get the value of the digital parameter.
  /// @return The value of the digital parameter.
  bool getValue() const { return value_; }

  /// @brief Set the sense bit of the digital parameter.
  /// @param sense - The sense bit of the digital parameter.
  void setSense(bool sense) { sense_ = sense; }

  /// @brief Get the sense bit of the digital parameter.
  /// @return The sense bit of the digital parameter.
  bool getSense() const { return sense_; }

  /// @brief Set the name of the digital parameter.
  /// @param name - The name of the digital parameter.
  /// @throws signal_easel::Exception if name for a given ID is invalid
  /// (length).

  /// @brief Set the name of the digital parameter.
  /// @param name - The name of the digital parameter.
  /// @throws signal_easel::Exception if name for a given ID is invalid
  /// (length).
  /// @note Does not validate characters.
  void setName(const std::string &name) {
    if (id_ == Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "Digital parameter ID not set.");
    }

    if (!validateDigitalDescriptor(name)) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "Invalid name length for analog parameter.");
    }

    name_ = name;
  }

  /// @brief Get the name of the digital parameter.
  /// @return The name of the digital parameter.
  std::string getName() const { return name_; }

  /// @brief Set the label of the digital parameter.
  /// @param label - The label of the digital parameter.
  void setLabel(const std::string &label) {
    if (id_ == Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "Digital parameter ID not set.");
    }

    if (!validateDigitalDescriptor(label)) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "Invalid unit/label length for digital parameter.");
    }

    label_ = label;
  }

  /// @brief Get the label of the digital parameter.
  /// @return The label of the digital parameter.
  std::string getLabel() const { return label_; }

private:
  /// @brief Validate the name, unit, or label of the parameter.
  /// @param value - The name, unit, or label of the parameter.
  /// @return \c true if the value is valid, \c false if not.
  bool validateDigitalDescriptor(const std::string &value) const {
    const size_t MAX_NAME_LENGTH_B1 = 5;
    const size_t MAX_NAME_LENGTH_B2 = 4;
    const size_t MAX_NAME_LENGTH_B3_B4_B5 = 3;
    const size_t MAX_NAME_LENGTH_B6_B7_B8 = 2;

    switch (id_) {
    case Id::B1:
      return value.size() <= MAX_NAME_LENGTH_B1;
    case Id::B2:
      return value.size() <= MAX_NAME_LENGTH_B2;
    case Id::B3:
    case Id::B4:
    case Id::B5:
      return value.size() <= MAX_NAME_LENGTH_B3_B4_B5;
    case Id::B6:
    case Id::B7:
    case Id::B8:
      return value.size() <= MAX_NAME_LENGTH_B6_B7_B8;
    default:
      return false;
    };
  }

  Id id_{Id::UNKNOWN};
  bool value_{false};
  bool sense_{true};
  std::string name_{};
  std::string label_{};
};

/// @brief A class to handle telemetry packets.
class TelemetryHandler {
public:
  TelemetryHandler() {
    for (size_t i = 0; i < analog_parameters_.size(); i++) {
      analog_parameters_.at(i).setId(static_cast<AnalogParameter::Id>(i));
    }
    for (size_t i = 0; i < digital_parameters_.size(); i++) {
      digital_parameters_.at(i).setId(static_cast<DigitalParameter::Id>(i));
    }
  }
  virtual ~TelemetryHandler() = default;

  void setAnalogValue(AnalogParameter::Id id, uint8_t value) {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "setAnalogValue called with UNKNOWN ID.");
    }
    analog_parameters_.at(static_cast<size_t>(id)).setRawValue(value);
  }

  uint8_t getAnalogValue(AnalogParameter::Id id) const {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "getAnalogValue called with UNKNOWN ID.");
    }
    return analog_parameters_.at(static_cast<size_t>(id)).getRawValue();
  }

  void setAnalogName(AnalogParameter::Id id, const std::string &name) {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "setAnalogName called with UNKNOWN ID.");
    }
    analog_parameters_.at(static_cast<size_t>(id)).setName(name);
  }

  std::string getAnalogName(AnalogParameter::Id id) const {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "getAnalogName called with UNKNOWN ID.");
    }
    return analog_parameters_.at(static_cast<size_t>(id)).getName();
  }

  void setAnalogUnit(AnalogParameter::Id id, const std::string &unit) {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "setAnalogUnitOrLabel called with UNKNOWN ID.");
    }
    analog_parameters_.at(static_cast<size_t>(id)).setUnit(unit);
  }

  std::string getAnalogUnit(AnalogParameter::Id id) const {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "getAnalogUnitOrLabel called with UNKNOWN ID.");
    }
    return analog_parameters_.at(static_cast<size_t>(id)).getUnit();
  }

  void setAnalogCoefficients(AnalogParameter::Id id, const std::string &a,
                             const std::string &b, const std::string &c) {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "setAnalogCoefficients called with UNKNOWN ID.");
    }
    analog_parameters_.at(static_cast<size_t>(id)).setCoefficients(a, b, c);
  }

  void getAnalogCoefficients(AnalogParameter::Id id, std::string &a,
                             std::string &b, std::string &c) const {
    if (id == AnalogParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "getAnalogCoefficients called with UNKNOWN ID.");
    }
    a = analog_parameters_.at(static_cast<size_t>(id)).getACoefficient();
    b = analog_parameters_.at(static_cast<size_t>(id)).getBCoefficient();
    c = analog_parameters_.at(static_cast<size_t>(id)).getCCoefficient();
  }

  void setDigitalValue(DigitalParameter::Id id, bool value) {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "setDigitalValue called with UNKNOWN ID.");
    }
    digital_parameters_.at(static_cast<size_t>(id)).setValue(value);
  }

  bool getDigitalValue(DigitalParameter::Id id) const {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "getDigitalValue called with UNKNOWN ID.");
    }
    return digital_parameters_.at(static_cast<size_t>(id)).getValue();
  }

  void setDigitalName(DigitalParameter::Id id, const std::string &name) {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "setDigitalName called with UNKNOWN ID.");
    }
    digital_parameters_.at(static_cast<size_t>(id)).setName(name);
  }

  std::string getDigitalName(DigitalParameter::Id id) const {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "getDigitalName called with UNKNOWN ID.");
    }
    return digital_parameters_.at(static_cast<size_t>(id)).getName();
  }

  void setDigitalLabel(DigitalParameter::Id id, const std::string &unit) {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "setDigitalUnitOrLabel called with UNKNOWN ID.");
    }
    digital_parameters_.at(static_cast<size_t>(id)).setLabel(unit);
  }

  std::string getDigitalLabel(DigitalParameter::Id id) const {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(
          Exception::Id::APRS_TELEMETRY,
          "getDigitalUnitOrLabel called with UNKNOWN ID.");
    }
    return digital_parameters_.at(static_cast<size_t>(id)).getLabel();
  }

  void setBitSense(DigitalParameter::Id id, bool sense) {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "setBitSense called with UNKNOWN ID.");
    }
    digital_parameters_.at(static_cast<size_t>(id)).setSense(sense);
  }

  bool getBitSense(DigitalParameter::Id id) const {
    if (id == DigitalParameter::Id::UNKNOWN) {
      throw signal_easel::Exception(Exception::Id::APRS_TELEMETRY,
                                    "getBitSense called with UNKNOWN ID.");
    }
    return digital_parameters_.at(static_cast<size_t>(id)).getSense();
  }

  /// @brief Set the sequence number of the telemetry data report packets.
  /// @param sequence_number - A value between 0 and 999. If the value is
  /// outside of this range, it will be reset to 0.
  bool setSequenceNumber(uint16_t sequence_number) {
    constexpr size_t SEQUENCE_NUMBER_MAX = 999;

    if (sequence_number > SEQUENCE_NUMBER_MAX) {
      sequence_number_ = 0;
      return false;
    }

    sequence_number_ = sequence_number;
    return true;
  }

  /// @brief Get the sequence number of the telemetry data report packets.
  /// @return The sequence number of the telemetry data report packets.
  uint16_t getSequenceNumber() const { return sequence_number_; }

  /// @brief Set the comment of telemetry data report packets.
  /// @param comment - A string of up to 220 characters, ASCII only but not
  /// checked.
  /// @return \c true if the comment was set, \c false if not.
  bool setComment(const std::string &comment) {
    if (comment.size() > COMMENT_MAX_LENGTH_) {
      return false;
    }

    comment_ = comment;
    return true;
  }

  /// @brief Get the comment of telemetry data report packets.
  std::string getComment() const { return comment_; }

  /// @brief Set the project title of telemetry bit sense packets.
  /// @param project_title - A string of up to 23 characters, ASCII only but not
  /// @return \c true if the project title was set, \c false if not.
  bool setProjectTitle(const std::string &project_title) {
    constexpr size_t PROJECT_TITLE_MAX_LENGTH = 23;
    if (project_title.size() > PROJECT_TITLE_MAX_LENGTH) {
      return false;
    }

    project_title_ = project_title;
    return true;
  }

  /// @brief Get the project title of telemetry bit sense packets.
  std::string getProjectTitle() const { return project_title_; }

  /// @brief Encode the telemetry report packet as a vector of bytes.
  /// @return The packet as a vector of bytes.
  std::vector<uint8_t> encodeDataReportMessage() {
    std::vector<uint8_t> packet;

    // Add the message type
    packet.push_back('T');

    // Add the sequence number
    packet.push_back('#');
    packet.push_back(sequence_number_ / 100 + '0');
    packet.push_back(sequence_number_ / 10 % 10 + '0');
    packet.push_back(sequence_number_ % 10 + '0');
    packet.push_back(',');

    // Add the analog parameters
    for (const auto &analog : analog_parameters_) {
      packet.push_back(analog.getRawValue() / 100 + '0');
      packet.push_back(analog.getRawValue() / 10 % 10 + '0');
      packet.push_back(analog.getRawValue() % 10 + '0');
      packet.push_back(',');
    }

    // Add the digital parameters
    for (const auto &digital : digital_parameters_) {
      packet.push_back(digital.getValue() ? '1' : '0');
    }

    // Add the comment
    for (char c : comment_) {
      packet.push_back(c);
    }

    return packet;
  }

  /// @brief Encode the telemetry parameter name message as a vector of bytes.
  /// @return The packet as a vector of bytes.
  std::vector<uint8_t> encodeParameterNameMessage() {
    std::vector<uint8_t> packet;

    // Add the message type
    packet.push_back('P');
    packet.push_back('A');
    packet.push_back('R');
    packet.push_back('M');
    packet.push_back('.');

    // Store a position to the last parameter with a name
    size_t last_param_with_name_pos = packet.size();

    // Add the analog parameters
    bool first = true;
    for (const auto &analog : analog_parameters_) {
      if (!first) {
        packet.push_back(',');
      }
      first = false;

      for (char c : analog.getName()) {
        packet.push_back(c);
      }

      if (!analog.getName().empty()) {
        last_param_with_name_pos = packet.size();
      }
    }

    // Add the digital parameters
    for (const auto &digital : digital_parameters_) {
      packet.push_back(',');

      for (char c : digital.getName()) {
        packet.push_back(c);
      }

      if (!digital.getName().empty()) {
        last_param_with_name_pos = packet.size();
      }
    }

    // Trim the packet to the last parameter with a name
    // packet.erase(last_param_with_name_pos, packet.end());

    return std::vector<uint8_t>(packet.begin(),
                                packet.begin() + last_param_with_name_pos);
  }

  /// @brief Encode the telemetry equation coefficients message as a vector of
  /// bytes.
  /// @return The packet as a vector of bytes.
  std::vector<uint8_t> encodeEquationCoefficientsMessage() {
    std::vector<uint8_t> packet;

    // Add the message type
    packet.push_back('E');
    packet.push_back('Q');
    packet.push_back('N');
    packet.push_back('S');
    packet.push_back('.');

    // Add the equation coefficients for each analog parameter
    bool first = true;
    for (const auto &analog : analog_parameters_) {
      if (!first) {
        packet.push_back(',');
      }
      first = false;

      for (char c : analog.getACoefficient()) {
        packet.push_back(c);
      }
      packet.push_back(',');

      for (char c : analog.getBCoefficient()) {
        packet.push_back(c);
      }
      packet.push_back(',');

      for (char c : analog.getCCoefficient()) {
        packet.push_back(c);
      }
    }

    return packet;
  }

  /// @brief Encode the telemetry parameter name message as a vector of bytes.
  /// @return The packet as a vector of bytes.
  std::vector<uint8_t> encodeUntilAndLabelMessage() {
    std::vector<uint8_t> packet;

    // Add the message type
    packet.push_back('U');
    packet.push_back('N');
    packet.push_back('I');
    packet.push_back('T');
    packet.push_back('.');

    // Store a position to the last parameter with a name
    size_t last_param_with_name_pos = packet.size();

    // Add the analog parameters
    bool first = true;
    for (const auto &analog : analog_parameters_) {
      if (!first) {
        packet.push_back(',');
      }
      first = false;

      for (char c : analog.getUnit()) {
        packet.push_back(c);
      }

      if (!analog.getUnit().empty()) {
        last_param_with_name_pos = packet.size();
      }
    }

    // Add the digital parameters
    for (const auto &digital : digital_parameters_) {
      packet.push_back(',');

      for (char c : digital.getLabel()) {
        packet.push_back(c);
      }

      if (!digital.getLabel().empty()) {
        last_param_with_name_pos = packet.size();
      }
    }

    // Trim the packet to the last parameter with a name
    // packet.erase(last_param_with_name_pos, packet.end());

    return std::vector<uint8_t>(packet.begin(),
                                packet.begin() + last_param_with_name_pos);
  }

  /// @brief Encode the telemetry bit sense message as a vector of bytes.
  /// @return The packet as a vector of bytes.
  std::vector<uint8_t> encodeBitSenseProjectNameMessage() {
    std::vector<uint8_t> packet;

    // Add the message type
    packet.push_back('B');
    packet.push_back('I');
    packet.push_back('T');
    packet.push_back('S');
    packet.push_back('.');

    // Add the digital parameters
    for (const auto &digital : digital_parameters_) {
      packet.push_back(digital.getSense() ? '1' : '0');
    }

    if (!project_title_.empty()) {
      packet.push_back(',');
      for (char c : project_title_) {
        packet.push_back(c);
      }
    }

    return packet;
  }

  bool decodeMessage(const std::vector<uint8_t> &message) {
    if (message.empty()) {
      return false;
    }

    switch (message.at(0)) {
    case 'T':
      return decodeDataReportMessage(message);
    case 'P':
      return decodeParameterDescriptor(message, true);
    case 'U':
      return decodeParameterDescriptor(message, false);
    case 'E':
      return decodeEquationCoefficientsMessage(message);
    case 'B':
      return decodeBitSenseProjectNameMessage(message);
    default:
      return false;
    };
  }

private:
  static constexpr size_t COMMENT_MAX_LENGTH_ = 220;

  /// @brief Validate the first few bytes of a message.
  /// @param message - The message to validate.
  /// @param header - The header to validate against, ex: "PARM."
  /// @return \c true if the header matches, \c false if not.
  bool decodeValidateHeader(const std::vector<uint8_t> &message,
                            const std::string &header) {
    if (message.size() < header.size()) {
      return false;
    }

    for (size_t i = 0; i < header.size(); i++) {
      if (message.at(i) != header.at(i)) {
        return false;
      }
    }

    return true;
  }

  bool decodeDataReportMessage(const std::vector<uint8_t> &message) {
    constexpr size_t MINIMUM_DATA_REPORT_MESSAGE_LENGTH =
        33; // In the case of "MIC" and not comma following it.
    if (message.size() < MINIMUM_DATA_REPORT_MESSAGE_LENGTH) {
      return false;
    }

    // Helper to check if a character is a digit
    auto isDigit = [](uint8_t c) { return c >= '0' && c <= '9'; };

    size_t pos = 0;

    // Check for T#
    if (message.at(pos++) != 'T' || message.at(pos++) != '#') {
      return false;
    }

    // Get the sequence number or MIC
    if (message.at(pos) == 'M' && message.at(pos + 1) == 'I' &&
        message.at(pos + 2) == 'C') {
      sequence_number_ = 0; // Just set it to 0.
      pos += 3;

      // MIC may or may not have a comma after it
      if (message.at(pos) != ',') {
        pos++;
      }
    } else {
      if (!isDigit(message.at(pos)) || !isDigit(message.at(pos + 1)) ||
          !isDigit(message.at(pos + 2)) || message.at(pos + 3) != ',') {
        return false;
      }

      sequence_number_ = (message.at(pos) - '0') * 100 +
                         (message.at(pos + 1) - '0') * 10 +
                         (message.at(pos + 2) - '0');

      pos += 4;
    }

    // Get the analog parameters
    for (auto &analog_parameters_ : analog_parameters_) {

      // Ensure the next three characters are digits
      if (!isDigit(message.at(pos)) || !isDigit(message.at(pos + 1)) ||
          !isDigit(message.at(pos + 2))) {
        return false;
      }

      analog_parameters_.setRawValue((message.at(pos) - '0') * 100 +
                                     (message.at(pos + 1) - '0') * 10 +
                                     (message.at(pos + 2) - '0'));

      pos += 3;

      // Trailing comma
      if (message.at(pos++) != ',') {
        return false;
      }
    }

    // Get the digital parameters
    for (auto &digital : digital_parameters_) {
      uint8_t bit_char = message.at(pos++);
      if (bit_char == '1') {
        digital.setValue(true);
      } else if (bit_char == '0') {
        digital.setValue(false);
      } else {
        return false;
      }
    }

    // Get the comment, if it exists
    comment_ = "";
    while (pos < message.size() &&
           pos < MINIMUM_DATA_REPORT_MESSAGE_LENGTH + COMMENT_MAX_LENGTH_) {
      comment_ += message.at(pos++);
    }

    return true;
  }

  /// @brief Decode a name or unit/label message. They're the same format.
  /// @param message - The message to decode.
  /// @param name_or_unit - \c true if decoding a name, \c false if decoding a
  /// unit or label.
  /// @return \c true if the message was decoded, \c false if not.
  bool decodeParameterDescriptor(const std::vector<uint8_t> &message,
                                 const bool name_or_unit) {
    constexpr size_t MINIMUM_PARAMETER_NAME_OR_UNIT_MESSAGE_LENGTH =
        5 + 1; // PARM.x

    if (message.size() < MINIMUM_PARAMETER_NAME_OR_UNIT_MESSAGE_LENGTH) {
      return false;
    }

    const std::string EXPECTED_HEADER = name_or_unit ? "PARM." : "UNIT.";
    if (!decodeValidateHeader(message, EXPECTED_HEADER)) {
      return false;
    }

    size_t pos = EXPECTED_HEADER.size();

    // Make sure that the first parameter has a value and isn't skipped
    if (message.at(pos) == ',') {
      return false;
    }

    // First attempt to decode the analog parameter names
    for (auto &analog : analog_parameters_) {
      // If there is data available, read it, otherwise the descriptor is empty
      // which is expected.
      std::string descriptor = "";
      while (pos < message.size()) {
        // Check for a comma
        if (message.at(pos) == ',') {
          pos++;
          break;
        }

        descriptor += message.at(pos++);
      }

      name_or_unit ? analog.setName(descriptor) : analog.setUnit(descriptor);
    }

    // Now do the same for the digital parameters
    for (auto &digital : digital_parameters_) {
      // If there is data available, read it, otherwise the descriptor is empty
      // which is expected.
      std::string descriptor = "";
      while (pos < message.size()) {
        // Check for a comma
        if (message.at(pos) == ',') {
          pos++;
          break;
        }

        descriptor += message.at(pos++);
      }

      name_or_unit ? digital.setName(descriptor) : digital.setLabel(descriptor);
    }

    return true;
  }

  bool decodeEquationCoefficientsMessage(const std::vector<uint8_t> &message) {
    constexpr size_t MINIMUM_EQUATION_COEFFICIENTS_MESSAGE_LENGTH = 5 + 29;
    if (message.size() < MINIMUM_EQUATION_COEFFICIENTS_MESSAGE_LENGTH) {
      return false;
    }

    const std::string EXPECTED_HEADER = "EQNS.";
    if (!decodeValidateHeader(message, EXPECTED_HEADER)) {
      return false;
    }

    size_t pos = EXPECTED_HEADER.size();

    // Get the equation coefficients for each analog parameter
    for (auto &analog : analog_parameters_) {
      std::string a = "";
      std::string b = "";
      std::string c = "";

      // Get the A coefficient
      while (pos < message.size() && message.at(pos) != ',') {
        a += message.at(pos++);
      }

      if (!AnalogParameter::validateCoefficient(a)) {
        return false;
      }

      // Skip the comma
      pos++;

      // Get the B coefficient
      while (pos < message.size() && message.at(pos) != ',') {
        b += message.at(pos++);
      }

      if (!AnalogParameter::validateCoefficient(b)) {
        return false;
      }

      // Skip the comma
      pos++;

      // Get the C coefficient
      while (pos < message.size() && message.at(pos) != ',') {
        c += message.at(pos++);
      }

      if (!AnalogParameter::validateCoefficient(c)) {
        return false;
      }

      // Skip the comma
      pos++;

      analog.setCoefficients(a, b, c);
    }

    return true;
  }

  bool decodeBitSenseProjectNameMessage(const std::vector<uint8_t> &message) {
    constexpr size_t MINIMUM_BIT_SENSE_MESSAGE_LENGTH = 5 + 8;
    if (message.size() < MINIMUM_BIT_SENSE_MESSAGE_LENGTH) {
      return false;
    }

    const std::string EXPECTED_HEADER = "BITS.";
    if (!decodeValidateHeader(message, EXPECTED_HEADER)) {
      return false;
    }

    size_t pos = EXPECTED_HEADER.size();

    // Get the bit sense values
    for (auto &digital : digital_parameters_) {
      if (pos >= message.size()) {
        return false;
      }

      if (message.at(pos) == '1') {
        digital.setSense(true);
      } else if (message.at(pos) == '0') {
        digital.setSense(false);
      } else {
        return false;
      }

      pos++;
    }

    // Get the project title if it exists
    if (pos < message.size()) {
      project_title_ = "";

      // Not clearly defined in the spec, but the example shows a comma
      if (message.at(pos) == ',') {
        pos++;
      }

      for (; pos < message.size(); pos++) {
        project_title_ += message.at(pos);
      }
    }

    return true;
  }

  uint16_t sequence_number_ = 0;
  std::string comment_ = "";
  std::string project_title_ = "";

  std::array<AnalogParameter, 5> analog_parameters_{};
  std::array<DigitalParameter, 8> digital_parameters_{};
};

} // namespace signal_easel::aprs
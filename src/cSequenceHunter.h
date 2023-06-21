class mcell : public cell::cCell
{
public:
    std::string value;

    mcell()
        : cell::cCell()
    {
        value = "?";
    }

    virtual std::string text()
    {
        return value + " ";
    }
};

class cSequence
{
public:
    cSequence(
        cell::cAutomaton<mcell> *matrix,
        const std::vector<int> &vid);

    bool firstStep(
        int &col,
        int &row,
        bool &vert) const;
    bool lastStep(
        int &col,
        int &row,
        bool &vert) const;
    void display() const;

private:
    cell::cAutomaton<mcell> *matrix;
    std::vector<int> myID; // visited cell IDs
    enum class eStepType
    {
        step,
        wm,
    };
    std::vector<eStepType> myStepType;
};

class cSequenceHunter
{
public:
    cSequenceHunter();

    void read(const std::string &fname);

    std::vector<int> findSequence(
        int seqNo);

    bool makePath(const std::vector<int> &order);

    int sequenceCount() const;

    std::vector<int> connect(
        const std::vector<int> &seq1,
        const std::vector<int> &seq2) const;

    int countSteps(
        const std::vector<std::vector<int>> &vSeq) const;

    void displayMatrix() const;
    void displayFinal() const;
    void displaySequence(int seqNo) const;
    void displayFoundSequence(
        const std::vector<int> &foundSequence) const;
    void displayCell(int id) const;

    std::vector<int> getPath() const
    {
        return path;
    }

private:
    cell::cAutomaton<mcell> *matrix;                 /// input matrix of numbers
    std::vector<std::vector<std::string>> vSequence; /// input sequences
    int maxPathLength;                               /// input maximum path length

    std::vector<std::vector<int>> vInitialWasted; /// initial wasted steps for each sequence path
    std::vector<std::vector<int>> vSequencePath;  /// path through each sequence

    std::vector<int> path; /// path to and through all sequences

    void SetMatrix(
        const std::vector<std::vector<std::string>> &vv);

    /// @brief Find sequence with given start point
    /// @param[in] seqNo index of sequence sought
    /// @param[in] foundSequence vector with index of starting cell
    /// @param[out] foundSequence vector with indices of cells in sequence, or empty on failure
    /// @param[in] vert true if previous move was vertical
    /// @return true if sequence found

    bool findSequenceFromStart(
        int seqNo,
        std::vector<int> &foundSequence,
        bool vert);

    /// @brief find 'wasted moves' required to reach sequence start from first row
    /// @param foundSequence
    /// @return vector of cell indices needed as stepping stones to sequence start

    std::vector<int> wastedMoves(
        std::vector<int> &foundSequence);

    std::vector<std::string>
    tokenize(
        const std::string &line);
};

/// @brief Combine overlapping sequences
/// @param seq1
/// @param seq2
/// @return if overlap exists, combined sequence.  If no overlap, {}
///
/// e.g. 1,2,3 and 2,3 4 => 1,2,3,4
std::vector<int> overlap(
    const std::vector<int> &seq1,
    const std::vector<int> &seq2);

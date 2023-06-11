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
    void read(const std::string &fname);

    std::vector<int> findSequence(int seqNo);

    int sequenceCount() const;

    std::vector<int> connect(
        std::vector<int> &seq1,
        std::vector<int> &seq2);

    void displayMatrix() const;
    void displaySequence(int seqNo) const;
    void displayFoundSequence(
        const std::vector<int> &foundSequence) const;

private:
    cell::cAutomaton<mcell> *matrix;
    std::vector<std::vector<std::string>> vSequence;

    /// @brief Find sequence with given start point
    /// @matrixram[in] seqNo index of sequence sought
    /// @matrixram[in] foundSequence vector with index of starting cell
    /// @matrixram[out] foundSequence vector with indices of cells in sequence, or empty on failure
    /// @matrixram[in] vert true if previous move was vertical
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


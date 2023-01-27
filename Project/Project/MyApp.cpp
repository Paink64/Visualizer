#include "MyApp.h"
#include "imgui.h"
#include <algorithm>
#include <random>
#include <time.h>
#include <queue>
#include <stack>
#include <vector>
#include <array>


namespace ImGui { extern ImGuiKeyData* GetKeyData(ImGuiKey key); }
namespace MyApp
{
    //Test if value is valid the the point
    bool IsValid(int Board[9][9], int row, int col, int test) {

        for (int i = 0; i < 9; i++)
        {
            if ((i != col && Board[row][i] == test) || (i != row && Board[i][col] == test))
                return false;
        }
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (Board[((row / 3) * 3) + i][((col / 3) * 3) + j] == test && (((col / 3) * 3) + j != col || ((row / 3) * 3) + i != row))
                    return false;
            }
        }
        return true;

    }

    static int project_type = 0;

    const char* program_names[] =
    {
        "Path Finder","Nth Queen Problem","Sorter","Sudoku"
    };

    void RenderUI()
    {
        #ifdef IMGUI_DISABLE_OBSOLETE_KEYIO
                struct funcs { static bool IsLegacyNativeDupe(ImGuiKey) { return false; } };
        #else
                struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
        #endif

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        switch (project_type)
        {
        case 0:         //Path Finder
        {
            ImGui::Begin("Path Finder");

            const ImU8  min = 1,                    //Sliders Max and Min values
                u8_Premin = 3,
                u8_Premax = 160,
                u8_Sizemin = 6,
                u8_Sizemax = 255;

            static bool stop = true,
                setup = true,
                Wall[256][256],
                Weight[256][256],
                PathT[256][256];

            static int  Presz = 9,
                sz = 75,
                steplength = 0,
                item_type = 0,
                click_type = 0,
                currentItem = item_type,
                Start[2] = { 256, 256 },
                End[2] = { 256, 256 },
                Hvalues[256][256],
                Fvalues[256][256];

            const ImVec2 p = ImGui::GetCursorScreenPos();

            const ImU32 colW = ImColor(ImVec4(1.0f, 1.0f, 1.0f, 255.0f)),
                colB = ImColor(ImVec4(0.0f, 0.0f, 0.0f, 255.0f));

            float   x = p.x + 4.0f,
                y = p.y + 4.0f;

            static float    FixedX = 9,
                FixedY = 9;

            static std::vector<std::array<int, 2>> FinalPath = {};

            static std::queue<std::vector<std::array<int, 2>>>  QueuePath = {};

            static std::stack<std::vector<std::array<int, 2>>>  StackPath = {};
            //Initialize all arrays
            if (setup)
            {
                for (int i = 0; i < 256; i++)
                {
                    for (int j = 0; j < 256; j++)
                    {
                        Weight[i][j] = false;
                        Wall[i][j] = false;
                        PathT[i][j] = false;
                        Hvalues[i][j] = 512;
                        Fvalues[i][j] = 512;
                    }
                }
                setup = false;
            }
            //If Search Begins
            if (!stop)
            {
                switch (item_type)
                {
                case 0:                                                                 //Breadth-first search
                {
                    std::vector<std::array<int, 2>> CurrentPath;
                    if (QueuePath.size() == 0) {                                        //When starting search set starting node as first point
                        CurrentPath.push_back({ Start[0], Start[1] });
                        PathT[Start[0]][Start[1]] = true;
                        QueuePath.push(CurrentPath);
                    }
                    CurrentPath = QueuePath.front();                                    //Set path at front of queue as current path
                    QueuePath.pop();

                    int TestHValues[4][2];                                              //Set horicontal and vertical nodes as childs
                    TestHValues[0][0] = CurrentPath.back()[0] - 1;
                    TestHValues[0][1] = CurrentPath.back()[1];

                    TestHValues[1][0] = CurrentPath.back()[0];
                    TestHValues[1][1] = CurrentPath.back()[1] + 1;

                    TestHValues[2][0] = CurrentPath.back()[0] + 1;
                    TestHValues[2][1] = CurrentPath.back()[1];

                    TestHValues[3][0] = CurrentPath.back()[0];
                    TestHValues[3][1] = CurrentPath.back()[1] - 1;

                    std::vector<std::array<int, 2>> TempPath;
                    for (int i = 0; i < 4; i++)
                    {
                        TempPath = CurrentPath;
                        if (Wall[TestHValues[i][0]][TestHValues[i][1]] == false && PathT[TestHValues[i][0]][TestHValues[i][1]] == false && TestHValues[i][1] < Presz && TestHValues[i][1] > -1 && TestHValues[i][0] < Presz && TestHValues[i][0] > -1)  //Check if new point is within Board size and not a wall or already searched node
                        {
                            TempPath.push_back({ TestHValues[i][0], TestHValues[i][1] });                   //Push point to end of current path
                            if (TempPath.back()[0] == End[0] && TempPath.back()[1] == End[1])               //If new point is the end stop search
                            {
                                FinalPath = TempPath;
                                steplength = FinalPath.size();
                                stop = true;
                                break;
                            }
                            QueuePath.push(TempPath);                                                       //Push new path to queue and set path as explored
                            PathT[TestHValues[i][0]][TestHValues[i][1]] = true;
                        }
                    }
                    break;
                }
                case 1:                                                                                     //Depth-first search
                {
                    std::vector<std::array<int, 2>> CurrentPath;
                    if (StackPath.size() == 0) {                                                            //When starting search set starting node as first point     
                        CurrentPath.push_back({ Start[0], Start[1] });
                        PathT[Start[0]][Start[1]] = true;
                        StackPath.push(CurrentPath);
                    }
                    CurrentPath = StackPath.top();                                                          //Set path at top of stack as current path

                    int TestHValues[4][2];                                                                  //Set horicontal and vertical nodes as childs
                    TestHValues[0][0] = CurrentPath.back()[0] - 1;
                    TestHValues[0][1] = CurrentPath.back()[1];

                    TestHValues[1][0] = CurrentPath.back()[0];
                    TestHValues[1][1] = CurrentPath.back()[1] + 1;

                    TestHValues[2][0] = CurrentPath.back()[0] + 1;
                    TestHValues[2][1] = CurrentPath.back()[1];

                    TestHValues[3][0] = CurrentPath.back()[0];
                    TestHValues[3][1] = CurrentPath.back()[1] - 1;

                    bool other = true;
                    std::vector<std::array<int, 2>> TempPath;
                    TempPath = CurrentPath;
                    for (int i = 0; i < 4; i++)
                    {
                        if (Wall[TestHValues[i][0]][TestHValues[i][1]] == false && PathT[TestHValues[i][0]][TestHValues[i][1]] == false && TestHValues[i][1] < Presz && TestHValues[i][1] > -1 && TestHValues[i][0] < Presz && TestHValues[i][0] > -1)
                        {
                            TempPath.push_back({ TestHValues[i][0], TestHValues[i][1] });                   //Push point to end of current path
                            if (TempPath.back()[0] == End[0] && TempPath.back()[1] == End[1])               //If new point is the end stop search
                            {
                                FinalPath = TempPath;
                                steplength = FinalPath.size();
                                stop = true;
                                break;
                            }
                            StackPath.push(TempPath);                                                       //Push new path to top of stack and set path as explored
                            PathT[TestHValues[i][0]][TestHValues[i][1]] = true;
                            other = false;
                            break;
                        }
                    }
                    if (other)                                                                              //If none of the paths were explored pop top of the stack
                        StackPath.pop();
                    break;
                }
                case 2:                                                                 //Dijkstra's Algorithm
                {
                    std::vector<std::array<int, 2>> CurrentPath;
                    if (QueuePath.size() == 0) {                                                            //When starting search set starting node as first point and initialize Hvalues 
                        CurrentPath.push_back({ Start[0], Start[1] });
                        QueuePath.push(CurrentPath);
                        PathT[CurrentPath.back()[0]][CurrentPath.back()[1]] = true;
                        for (int i = 0; i < Presz; i++)
                        {
                            for (int j = 0; j < Presz; j++)
                            {
                                Hvalues[i][j] = 512;
                            }
                        }
                        Hvalues[Start[0]][Start[1]] = 0;
                    }
                    CurrentPath = QueuePath.front();
                    QueuePath.pop();

                    int LowestValue = Hvalues[CurrentPath.back()[0]][CurrentPath.back()[1]];            //Get path with lowest cost from starting point
                    for (int i = 0; i < QueuePath.size(); i++)
                    {
                        int ComprHvalue = Hvalues[QueuePath.front().back()[0]][QueuePath.front().back()[1]];

                        if (LowestValue > ComprHvalue)
                        {
                            QueuePath.push(CurrentPath);
                            CurrentPath = QueuePath.front();
                            QueuePath.pop();
                            LowestValue = ComprHvalue;
                        }
                        else
                        {
                            QueuePath.push(QueuePath.front());
                            QueuePath.pop();
                        }
                    }
                    PathT[CurrentPath.back()[0]][CurrentPath.back()[1]] = true;
                    if (CurrentPath.back()[0] == End[0] && CurrentPath.back()[1] == End[1])         //If current path leads to end stop search
                    {
                        FinalPath = CurrentPath;
                        steplength = FinalPath.size();
                        stop = true;
                        break;
                    }
                    int TestHValues[4][2];                                                          //Set horicontal and vertical nodes as childs
                    TestHValues[0][0] = CurrentPath.back()[0] - 1;
                    TestHValues[0][1] = CurrentPath.back()[1];


                    TestHValues[1][0] = CurrentPath.back()[0];
                    TestHValues[1][1] = CurrentPath.back()[1] + 1;


                    TestHValues[2][0] = CurrentPath.back()[0] + 1;
                    TestHValues[2][1] = CurrentPath.back()[1];

                    TestHValues[3][0] = CurrentPath.back()[0];
                    TestHValues[3][1] = CurrentPath.back()[1] - 1;
                    LowestValue = 512;
                    std::vector<std::array<int, 2>> TempPath;
                    for (int i = 0; i < 4; i++)
                    {
                        TempPath = CurrentPath;
                        int TempValue = Hvalues[CurrentPath.back()[0]][CurrentPath.back()[1]] + 1;  //Calculate what the cost would be to go to next point
                        if (Weight[TestHValues[i][0]][TestHValues[i][1]])
                            TempValue += 10;
                        if (Hvalues[TestHValues[i][0]][TestHValues[i][1]] > TempValue && Wall[TestHValues[i][0]][TestHValues[i][1]] == false && TestHValues[i][1] < Presz && TestHValues[i][1] > -1 && TestHValues[i][0] < Presz && TestHValues[i][0] > -1)
                        {
                            Hvalues[TestHValues[i][0]][TestHValues[i][1]] = TempValue;          //Set Cost to node
                            TempPath.push_back({ TestHValues[i][0], TestHValues[i][1] });       //Add path to queue
                            QueuePath.push(TempPath);
                            int loopval = QueuePath.size() - 1;
                            for (int j = 0; j < loopval; j++)                                   //Delete any other path that lead to same node
                            {
                                if (QueuePath.front().back()[0] == TestHValues[i][0] && QueuePath.front().back()[1] == TestHValues[i][1])
                                {
                                    QueuePath.pop();
                                }
                                else
                                {
                                    QueuePath.push(QueuePath.front());
                                    QueuePath.pop();
                                }
                            }
                        }
                    }
                    break;
                }
                case 3:                                                                 //Greedy Algorithm
                {
                    std::vector<std::array<int, 2>> CurrentPath;
                    if (QueuePath.size() == 0) {                                                            //When starting search set starting node as first point 
                        CurrentPath.push_back({ Start[0], Start[1] });
                        QueuePath.push(CurrentPath);
                        for (int i = 0; i < Presz; i++)
                        {
                            for (int j = 0; j < Presz; j++)                                                 //Set Hvalues
                            {
                                Hvalues[i][j] = (abs(i - End[0]) + abs(j - End[1]));
                                if (Weight[i][j])
                                    Hvalues[i][j] += 10;
                            }
                        }
                    }
                    CurrentPath = QueuePath.front();
                    QueuePath.pop();
                    int LowestValue = Hvalues[CurrentPath.back()[0]][CurrentPath.back()[1]];            //Get path with lowest cost from end point
                    for (int i = 0; i < QueuePath.size(); i++)
                    {
                        int ComprHvalue = Hvalues[QueuePath.front().back()[0]][QueuePath.front().back()[1]];

                        if (LowestValue > ComprHvalue)
                        {
                            QueuePath.push(CurrentPath);
                            CurrentPath = QueuePath.front();
                            QueuePath.pop();
                            LowestValue = ComprHvalue;
                        }
                        else
                        {
                            QueuePath.push(QueuePath.front());
                            QueuePath.pop();
                        }
                    }
                    PathT[CurrentPath.back()[0]][CurrentPath.back()[1]] = true;
                    std::vector<std::array<int, 2>> TempPath;
                    int TestHValues[4][3];                                                                  //Set horicontal and vertical nodes as childs

                    TestHValues[0][0] = CurrentPath.back()[0];
                    TestHValues[0][1] = CurrentPath.back()[1] - 1;

                    TestHValues[1][0] = CurrentPath.back()[0] + 1;
                    TestHValues[1][1] = CurrentPath.back()[1];

                    TestHValues[2][0] = CurrentPath.back()[0] - 1;
                    TestHValues[2][1] = CurrentPath.back()[1];

                    TestHValues[3][0] = CurrentPath.back()[0];
                    TestHValues[3][1] = CurrentPath.back()[1] + 1;

                    for (int i = 0; i < 4; i++)
                    {
                        TempPath = CurrentPath;
                        if (Wall[TestHValues[i][0]][TestHValues[i][1]] == false && PathT[TestHValues[i][0]][TestHValues[i][1]] == false && TestHValues[i][1] < Presz && TestHValues[i][1] > -1 && TestHValues[i][0] < Presz && TestHValues[i][0] > -1)
                        {
                            bool insert = true;;
                            int loopval = QueuePath.size();
                            for (int j = 0; j < loopval; j++)                       //Check if node has not been discovered already
                            {
                                if (QueuePath.front().back()[0] == TestHValues[i][0] && QueuePath.front().back()[1] == TestHValues[i][1])
                                {
                                    insert = false;
                                }
                                else
                                {
                                    QueuePath.push(QueuePath.front());
                                    QueuePath.pop();
                                }
                            }
                            if (insert)                                                 //Push new path to queue is node has not been discovered already
                            {
                                TempPath.push_back({ TestHValues[i][0], TestHValues[i][1] });
                                if (TempPath.back()[0] == End[0] && TempPath.back()[1] == End[1])       //If current path leads to end stop search
                                {
                                    FinalPath = TempPath;
                                    steplength = FinalPath.size();
                                    stop = true;
                                    break;
                                }
                                QueuePath.push(TempPath);
                            }
                        }
                    }
                    break;
                }
                default:                                                                //A* Algorithm
                {
                    std::vector<std::array<int, 2>> CurrentPath;
                    if (QueuePath.size() == 0) {                                        //When starting search set starting node as first point 
                        CurrentPath.push_back({ Start[0], Start[1] });
                        QueuePath.push(CurrentPath);
                        for (int i = 0; i < Presz; i++)
                        {
                            for (int j = 0; j < Presz; j++)
                            {                                                           //Set Hvalues and Fvalues
                                Fvalues[i][j] = 512;
                                Hvalues[i][j] = (abs(i - End[0]) + abs(j - End[1]));
                            }
                        }
                        Fvalues[Start[0]][Start[1]] = Hvalues[Start[0]][Start[1]];
                    }
                    CurrentPath = QueuePath.front();
                    QueuePath.pop();
                    int LowestValue = Fvalues[CurrentPath.back()[0]][CurrentPath.back()[1]];                //Get path with Fvalue (Distance from end node - path cost from start node)
                    for (int i = 0; i < QueuePath.size(); i++)
                    {
                        int ComprHvalue = Fvalues[QueuePath.front().back()[0]][QueuePath.front().back()[1]];

                        if (LowestValue > ComprHvalue)
                        {
                            QueuePath.push(CurrentPath);
                            CurrentPath = QueuePath.front();
                            QueuePath.pop();
                            LowestValue = ComprHvalue;
                        }
                        else
                        {
                            QueuePath.push(QueuePath.front());
                            QueuePath.pop();
                        }
                    }
                    PathT[CurrentPath.back()[0]][CurrentPath.back()[1]] = true;
                    std::vector<std::array<int, 2>> TempPath;
                    int TestHValues[4][2];                                                                  //Set horicontal and vertical nodes as childs
                    TestHValues[0][0] = CurrentPath.back()[0];
                    TestHValues[0][1] = CurrentPath.back()[1] - 1;

                    TestHValues[1][0] = CurrentPath.back()[0] + 1;
                    TestHValues[1][1] = CurrentPath.back()[1];

                    TestHValues[2][0] = CurrentPath.back()[0] - 1;
                    TestHValues[2][1] = CurrentPath.back()[1];

                    TestHValues[3][0] = CurrentPath.back()[0];
                    TestHValues[3][1] = CurrentPath.back()[1] + 1;

                    for (int i = 0; i < 4; i++)
                    {
                        TempPath = CurrentPath;
                        TempPath.push_back({ TestHValues[i][0], TestHValues[i][1] });
                        if (TempPath.back()[0] == End[0] && TempPath.back()[1] == End[1])       //If current path leads to end stop search
                        {
                            FinalPath = TempPath;
                            steplength = FinalPath.size();
                            stop = true;
                            break;
                        }
                        int TempFValue = Fvalues[CurrentPath.back()[0]][CurrentPath.back()[1]] - Hvalues[CurrentPath.back()[0]][CurrentPath.back()[1]] + Hvalues[TestHValues[i][0]][TestHValues[i][1]] + 1;     //Estimate cost to node
                        if (Weight[TestHValues[i][0]][TestHValues[i][1]])
                            TempFValue += 10;
                        else if (Fvalues[TestHValues[i][0]][TestHValues[i][1]] > TempFValue && Wall[TestHValues[i][0]][TestHValues[i][1]] == false && TestHValues[i][1] < Presz && TestHValues[i][1] > -1 && TestHValues[i][0] < Presz && TestHValues[i][0] > -1) //If cose is less
                        {
                            Fvalues[TestHValues[i][0]][TestHValues[i][1]] = TempFValue;     //If insert path and delete any other path that leads to same node 
                            QueuePath.push(TempPath);
                            int loopval = QueuePath.size() - 1;
                            for (int j = 0; j < loopval; j++)
                            {
                                if (QueuePath.front().back()[0] == TestHValues[i][0] && QueuePath.front().back()[1] == TestHValues[i][1])
                                {
                                    QueuePath.pop();
                                }
                                else
                                {
                                    QueuePath.push(QueuePath.front());
                                    QueuePath.pop();
                                }
                            }
                        }
                    }
                    break;
                }
                }
            }

            ImGui::SetCursorScreenPos(ImVec2(x, y));
            for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
                if (ImGui::IsMouseDown(i))
                {
                    FixedY = (int)(io.MousePos.x - x) / sz;                                     //Get Mouse Position
                    FixedX = (int)(io.MousePos.y - y) / sz;
                    if ((int)FixedY < Presz && (int)FixedX < Presz)
                        switch (click_type)
                        {
                        case 0:                                                                 //Place Start Node And Remove Any Other Node
                        {
                            Wall[(int)FixedX][(int)FixedY] = false;
                            Weight[(int)FixedX][(int)FixedY] = false;
                            Start[0] = (int)FixedX;
                            Start[1] = (int)FixedY;
                            if (End[0] == (int)FixedX && End[1] == (int)FixedY)
                            {
                                End[0] = Presz;
                                End[1] = Presz;
                            }
                            currentItem = 5;
                            break;
                        }
                        case 1:                                                                 //Place End Node And Remove Any Other Node
                        {
                            Wall[(int)FixedX][(int)FixedY] = false;
                            Weight[(int)FixedX][(int)FixedY] = false;
                            End[0] = (int)FixedX;
                            End[1] = (int)FixedY;
                            if (Start[0] == (int)FixedX && Start[1] == (int)FixedY)
                            {
                                Start[0] = Presz;
                                Start[1] = Presz;
                            }
                            currentItem = 5;
                            break;
                        }
                        case 2:                                                                 //Place/Delete Weight Node
                        {
                            if (i == 0)                                                         //Left Click Place Weight And Remove Any Other Node
                            {
                                Weight[(int)FixedX][(int)FixedY] = true;
                                Wall[(int)FixedX][(int)FixedY] = false;
                                if (Start[0] == (int)FixedX && Start[1] == (int)FixedY)
                                {
                                    Start[0] = Presz;
                                    Start[1] = Presz;
                                }
                                if ((int)FixedX == End[0] && (int)FixedY == End[1])
                                {
                                    End[0] = Presz;
                                    End[1] = Presz;
                                }
                                currentItem = 5;
                            }
                            else if (i == 1)                                                    //Right Click Delete Weight
                            {
                                Weight[(int)FixedX][(int)FixedY] = false;
                                currentItem = 5;
                            }
                            break;
                        }
                        default:                                                                //Place/Delete Wall Node
                        {
                            if (i == 0)                                                         //Left Click Place Wall And Remove Any Other Node
                            {
                                Wall[(int)FixedX][(int)FixedY] = true;
                                Weight[(int)FixedX][(int)FixedY] = false;
                                if (Start[0] == (int)FixedX && Start[1] == (int)FixedY)
                                {
                                    Start[0] = Presz;
                                    Start[1] = Presz;
                                }
                                if ((int)FixedX == End[0] && (int)FixedY == End[1])
                                {
                                    End[0] = Presz;
                                    End[1] = Presz;
                                }
                                currentItem = 5;
                            }
                            else if (i == 1)                                                    //Right Click Delete Wall
                            {
                                Wall[(int)FixedX][(int)FixedY] = false;
                                currentItem = 5;
                            }
                            break;
                        }
                        }
                }

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + Presz * sz, y + Presz * sz), colW);
            for (int i = 0; i < Presz; i++)                                                                 // Draw each node on grid
            {
                for (int j = 0; j < Presz; j++)
                {
                    if (PathT[i][j])
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), ImColor(ImVec4(0.537f, 0.812f, 0.941f, 255.0f)));
                    if (FinalPath.size() != 0)
                        for (int k = 0; k < steplength; k++)
                        {
                            if (FinalPath[k][0] == i && FinalPath[k][1] == j)
                                draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), ImColor(ImVec4(1.0f, 1.0f, 0.0f, 255.0f)));
                        }
                    if (Start[0] == i && Start[1] == j)
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), ImColor(ImVec4(0.50f, 1.0f, 0.0f, 255.0f)));
                    if (End[0] == i && End[1] == j)
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), ImColor(ImVec4(1.0f, 0.0f, 0.0f, 255.0f)));
                    if (Wall[i][j])
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), colB);
                    if (Weight[i][j])
                        draw_list->AddCircleFilled(ImVec2(x + sz / 2, y + sz / 2), sz / 4, colB);
                    draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), colB, 0.0f, ImDrawFlags_None, sz / 16);
                    x += sz;
                }
                x = p.x + 4;
                y += sz;
            }

            ImGui::End();
            ImGui::Begin("Settings");

            ImGui::Combo(" Projects", &project_type, program_names, IM_ARRAYSIZE(program_names), IM_ARRAYSIZE(program_names));
            ImGui::NewLine();

            ImGui::SliderScalar("Board Size", ImGuiDataType_U8, &Presz, &u8_Premin, &u8_Premax, "%u");
            ImGui::SliderScalar("Scale", ImGuiDataType_U8, &sz, &u8_Sizemin, &u8_Sizemax, "%u");
            static int max;
            if (FinalPath.size() != 0)
            {
                max = FinalPath.size();
                ImGui::SliderScalar("Steps", ImGuiDataType_U8, &steplength, &min, &max, "%u");
            }
            else
                ImGui::NewLine();

            const char* item_names[] =
            {
                "Breadth-First Search", "Depth-First Search", "Dijkstras Algorithm", "Greedy Algorithm", "A* Algorithm"
            };

            ImGui::Combo(" Algorithm", &item_type, item_names, IM_ARRAYSIZE(item_names), IM_ARRAYSIZE(item_names));

            if (ImGui::Button("Start Search"))
            {
                if (Start[0] < Presz && End[0] < Presz)          //Start Search if Start node and End node are in grid
                    stop = false;
            }
            else if (ImGui::Button("Stop Search"))
            {
                stop = true;
            }
            else if (ImGui::Button("Restart Search") || currentItem != item_type)       //Reset variables that have to do with search
            {
                currentItem = item_type;
                stop = true;
                for (int i = 0; i < 256; i++)
                {
                    for (int j = 0; j < 256; j++)
                    {
                        PathT[i][j] = false;
                        Hvalues[i][j] = 512;
                    }
                }
                QueuePath = {};
                StackPath = {};
                FinalPath = {};
            }

            ImGui::NewLine();

            if (ImGui::Button("Start Node")) //Buttons change fuction of mouse click
            {
                click_type = 0;
            }
            else if (ImGui::Button("End Node"))
            {
                click_type = 1;
            }
            else if (ImGui::Button("Weight"))
            {
                click_type = 2;
            }
            else if (ImGui::Button("Wall"))
            {
                click_type = 3;
            }
            else if (ImGui::Button("Reset Board"))      //Reset all variables 
            {
                stop = true;
                for (int i = 0; i < 256; i++)
                {
                    for (int j = 0; j < 256; j++)
                    {
                        Wall[i][j] = false;
                        Weight[i][j] = false;
                        PathT[i][j] = false;
                        Hvalues[i][j] = 512;
                        Fvalues[i][j] = 512;
                    }
                }
                QueuePath = {};
                StackPath = {};
                FinalPath = {};
                Start[0] = 256;
                Start[1] = 256;
                End[0] = 256;
                End[1] = 256;
            }


            switch (click_type)
            {
            case 0:                                                                 //Start Node
            {
                ImGui::NewLine();
                ImGui::Text("CLICK ON GRID TO PLACE START NODE");
                break;
            }
            case 1:                                                                 //End Node
            {
                ImGui::NewLine();
                ImGui::Text("CLICK ON GRID TO PLACE END NODE");
                break;
            }
            case 2:                                                                 //Weight Node
            {
                ImGui::NewLine();
                ImGui::Text("LEFT-CLICK TO PLACE Weight");
                ImGui::Text("RIGHT-CLICK TO DELETE Weight");
                ImGui::Text("WEIGHTS ADD A COST OF 10 TO A NODE");
                break;
            }
            default:                                                                //Wall Node
            {
                ImGui::NewLine();
                ImGui::Text("LEFT-CLICK TO PLACE WALL");
                ImGui::Text("RIGHT-CLICK TO DELETE WALL");
                ImGui::Text("WALLS BLOCK POSSIBLE PATHS");
                break;
            }
            }

            ImGui::End();
            break;
        }
        case 1:         //Nth Queen Problem
        {
            ImGui::Begin("Nth Queen");
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            const ImVec2 p = ImGui::GetCursorScreenPos();

            const ImU8  u8_min = 4,
                u8_max = 32,
                u8_Sizemin = 30,
                u8_Sizemax = 240;

            const ImU32 colQ = ImColor(ImVec4(0.97f, 0.97f, 0.97f, 255.0f)),
                colB = ImColor(ImVec4(0.27f, 0.27f, 0.27f, 255.0f));

            static int  sz = 75,
                i = 1,
                size = 8,
                setup = -1,
                Board[32];

            float   x = p.x + 4.0f,
                y = p.y + 4.0f;

            static bool test = true,
                stop = true;

            static float value = 0.0f;

            if (setup != size) {
                stop = true;
                for (int n = 0; n < 32; n++)
                {
                    Board[n] = 32;
                }
                setup = size;
            }

            if (!stop)
                if (Board[i] < size) {      //Backtracking algorithm
                    if (i == 0) {
                        i++;
                        Board[i] = 0;
                    }
                    bool incim = true;
                    for (int j = 0; j < i; j++)
                    {
                        if ((Board[j] == Board[i]) || (abs(((float)j - (float)i) / ((float)Board[j] - (float)Board[i])) == 1)) // Checks if new queen position interferes with previous queens
                        {
                            Board[i]++;
                            incim = false;
                            break;
                        }
                    }
                    if (incim)      //If no problem was found start checking next row
                    {
                        i++;
                        Board[i] = 0;
                    }
                }
                else {      //If out of bound go back to last row and increment space
                    i--;
                    Board[i]++;
                }
            if (i == size) {
                stop = true;
            }

            for (int n = 0; n < size; n++)
            {
                for (int m = 0; m < size; m++)
                {
                    if ((m + n) % 2 == 0)       //Draw Board
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), ImColor(ImVec4(0.93f, 0.93f, 0.82f, 255.0f)));
                    else
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), ImColor(ImVec4(0.46f, 0.58f, 0.33f, 255.0f)));

                    if (Board[n] == m)          //Draw Queens
                    {
                        draw_list->AddCircleFilled(ImVec2(x + sz * .18, y + sz * .34), sz * .09, colQ, 40);
                        draw_list->AddCircleFilled(ImVec2(x + sz * .38, y + sz * .19), sz * .09, colQ, 40);
                        draw_list->AddCircleFilled(ImVec2(x + sz * .62, y + sz * .18), sz * .09, colQ, 40);
                        draw_list->AddCircleFilled(ImVec2(x + sz * .82, y + sz * .34), sz * .09, colQ, 40);
                        draw_list->AddCircle(ImVec2(x + sz * .18, y + sz * .34), sz * .09, colB, 40, 2);
                        draw_list->AddCircle(ImVec2(x + sz * .38, y + sz * .19), sz * .09, colB, 40, 2);
                        draw_list->AddCircle(ImVec2(x + sz * .62, y + sz * .18), sz * .09, colB, 40, 2);
                        draw_list->AddCircle(ImVec2(x + sz * .82, y + sz * .34), sz * .09, colB, 40, 2);

                        //White part of Queen
                        draw_list->AddLine(ImVec2(x + sz * .18, y + sz * .34), ImVec2(x + sz * .3868, y + sz * .86), colQ, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .17, y + sz * .34), ImVec2(x + sz * .60, y + sz * .86), colQ, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .39, y + sz * .19), ImVec2(x + sz * .3868, y + sz * .86), colQ, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .38, y + sz * .19), ImVec2(x + sz * .60, y + sz * .86), colQ, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .38, y + sz * .19), ImVec2(x + sz * .50, y + sz * .86), colQ, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .62, y + sz * .18), ImVec2(x + sz * .3868, y + sz * .86), colQ, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .62, y + sz * .18), ImVec2(x + sz * .60, y + sz * .86), colQ, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .62, y + sz * .18), ImVec2(x + sz * .50, y + sz * .86), colQ, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .82, y + sz * .35), ImVec2(x + sz * .38, y + sz * .86), colQ, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .82, y + sz * .35), ImVec2(x + sz * .59, y + sz * .86), colQ, sz / 16);
                        draw_list->AddRectFilled(ImVec2(x + sz * .21, y + sz * .80), ImVec2(x + sz * .78, y + sz * .88), colQ);
                        draw_list->AddRectFilled(ImVec2(x + sz * .30, y + sz * .88), ImVec2(x + sz * .68, y + sz * .50), colQ);
                        draw_list->AddRectFilled(ImVec2(x + sz * .37, y + sz * .43), ImVec2(x + sz * .62, y + sz * .88), colQ);
                        draw_list->AddRectFilled(ImVec2(x + sz * .27, y + sz * .65), ImVec2(x + sz * .72, y + sz * .50), colQ);
                        draw_list->AddLine(ImVec2(x + sz * .21, y + sz * .86), ImVec2(x + sz * .78, y + sz * .86), colQ, sz / 16);
                        draw_list->AddBezierCubic(ImVec2(x + sz * .232, y + sz * .84), ImVec2(x + sz * .30, y + sz * .7), ImVec2(x + sz * .68, y + sz * .7), ImVec2(x + sz * .763, y + sz * .84), colQ, sz / 16, 12);

                        //Queen Border
                        draw_list->AddLine(ImVec2(x + sz * .18, y + sz * .43), ImVec2(x + sz * .30, y + sz * .73), colB, 2);
                        draw_list->AddLine(ImVec2(x + sz * .25, y + sz * .39), ImVec2(x + sz * .35, y + sz * .50), colB, 2);
                        draw_list->AddLine(ImVec2(x + sz * .35, y + sz * .50), ImVec2(x + sz * .36, y + sz * .27), colB, 2);
                        draw_list->AddLine(ImVec2(x + sz * .43, y + sz * .25), ImVec2(x + sz * .50, y + sz * .43), colB, 2);
                        draw_list->AddLine(ImVec2(x + sz * .50, y + sz * .43), ImVec2(x + sz * .56, y + sz * .25), colB, 2);
                        draw_list->AddLine(ImVec2(x + sz * .65, y + sz * .26), ImVec2(x + sz * .65, y + sz * .49), colB, 2);
                        draw_list->AddLine(ImVec2(x + sz * .65, y + sz * .49), ImVec2(x + sz * .75, y + sz * .39), colB, 2);
                        draw_list->AddLine(ImVec2(x + sz * .50, y + sz * .43), ImVec2(x + sz * .56, y + sz * .25), colB, 2);
                        draw_list->AddLine(ImVec2(x + sz * .82, y + sz * .43), ImVec2(x + sz * .68, y + sz * .73), colB, 2);
                        draw_list->AddLine(ImVec2(x + sz * .21, y + sz * .89), ImVec2(x + sz * .78, y + sz * .89), colB, 2);
                        draw_list->AddLine(ImVec2(x + sz * .78, y + sz * .89), ImVec2(x + sz * .78, y + sz * .80), colB, 2);
                        draw_list->AddLine(ImVec2(x + sz * .21, y + sz * .89), ImVec2(x + sz * .21, y + sz * .80), colB, 2);
                        draw_list->AddBezierCubic(ImVec2(x + sz * .21, y + sz * .80), ImVec2(x + sz * .30, y + sz * .67), ImVec2(x + sz * .68, y + sz * .67), ImVec2(x + sz * .78, y + sz * .80), colB, 2, 12);
                    }
                    x += sz;
                }
                x = p.x + 4;
                y += sz;
            }

            ImGui::End();
            ImGui::Begin("Settings");

            ImGui::Combo(" Projects", &project_type, program_names, IM_ARRAYSIZE(program_names), IM_ARRAYSIZE(program_names));
            ImGui::NewLine();

            ImGui::SliderScalar("Size of Board", ImGuiDataType_U8, &size, &u8_min, &u8_max, "%u");
            ImGui::SliderScalar("Scale", ImGuiDataType_U8, &sz, &u8_Sizemin, &u8_Sizemax, "%u");

            if (ImGui::Button("Begin"))       //Start solver is starting from nothing initilize beginning
            {
                if (Board[0] > size) {
                    Board[0] = 0;
                    Board[1] = 2;
                }
                stop = false;
            }
            if (ImGui::Button("Stop"))
            {
                stop = true;
            }
            if (ImGui::Button("Restart"))       //Reset Board
            {
                stop = true;
                i = 1;
                for (int n = 0; n < 32; n++)
                {
                    Board[n] = 32;
                }
            }

            ImGui::End();
            break;
        }
        case 2:         //Sorter
        {
            ImGui::Begin("Sorter");

            const ImU8  u8_min = 2;

            static int  size = 50,
                        setup = -1,
                        item_type = 0,
                        current_item = 0,
                        i = 0,
                        j = 0,
                        k = 0,
                        l = 0,
                        left = 0,
                        right = 0,
                        tempLeft = 0,
                        tempRight = 0,
                        u8_max = 255,
                        HistoMin = 25,
                        HistoWidthMax = 1600,
                        HistoHeightMax = 950,
                        HistoWidth = 1000,
                        HistoHeight = 700;


            static float    arr[255],
                            tempArr[255];

            static bool stop = true,
                        flipped = false,
                        flipped2 = false;

            if (setup != size) {
                for (int i = 0; i < size; i++) {
                    arr[i] = i + 1;
                }
                setup = size;
            }

            ImGui::PlotHistogram("", arr, size, 0, NULL, 0.0f, size, ImVec2(HistoWidth, HistoHeight));
            ImGui::End();

            ImGui::Begin("Settings");
            ImGui::Combo(" Projects", &project_type, program_names, IM_ARRAYSIZE(program_names), IM_ARRAYSIZE(program_names));
            ImGui::NewLine();


            ImGui::SliderScalar("Height", ImGuiDataType_U16, &HistoHeight, &HistoMin, &HistoHeightMax, "%u");

            ImGui::SliderScalar("Width", ImGuiDataType_U16, &HistoWidth, &HistoMin, &HistoWidthMax, "%u");
            ImGui::SliderScalar("Size", ImGuiDataType_U8, &size, &u8_min, &u8_max, "%u");
            ImGui::NewLine();

            const char* item_names[] =
            {
                "Selection Sort","Gnome Sort","Bogo Sort","Bubble Sort","Cocktail Sort", "Insertion Sort","Comb Sort","Shell Sort","Merge Sort"
            };

            ImGui::Combo(" Algorithm", &item_type, item_names, IM_ARRAYSIZE(item_names), IM_ARRAYSIZE(item_names));
            ImGui::NewLine();

            if (ImGui::Button("Shuffle"))
            {
                for (int i = 0; i < size; i++) {
                    arr[i] = i + 1;
                }
                i = -1;
                stop = true;
                std::shuffle(arr, arr + size, std::default_random_engine(time(NULL)));
            }
            else if (ImGui::Button("Begin"))
            {
                stop = false;
            }
            else if (ImGui::Button("Stop"))
            {
                stop = true;
            }

            if (current_item != item_type) {
                stop = true;
                i = -1;
                current_item = item_type;
            }

            if (!stop)
            {
                ImGui::NewLine();
                ImGui::Text("Sorting");
                switch (item_type)
                {
                case 0:         //Selection Sort
                {
                    if (i == -1) {
                        i = 0;
                        j = 1;
                        k = 0;
                    }
                    if ((j == size))
                    {
                        if (k != i) {
                            int temp = arr[k];
                            arr[k] = arr[i];
                            arr[i] = temp;
                        }
                        i++;
                        j = i + 1;
                        k = i;
                    }
                    if (i == size - 1) {
                        stop = true;
                        break;
                    }
                    if (arr[j] < arr[k]) {
                        k = j;
                    }
                    j++;
                    break;
                }
                case 1:         //Gnome Sort
                {
                    if (i == -1) {
                        i = 1;
                    }
                    if (i == size)
                    {
                        stop = true;
                        break;
                    }
                    if (i == 0) {
                        i++;
                    }
                    else if (arr[i] < arr[i - 1])
                    {
                        int temp = arr[i];
                        arr[i] = arr[i - 1];
                        arr[i - 1] = temp;
                        i--;
                    }
                    else
                        i++;
                    break;
                }
                case 2:         //Bogo Sort
                {
                    std::shuffle(arr, arr + size, std::default_random_engine(time(NULL)));
                    for (int i = 0; i < size; i++)
                    {
                        if (arr[i] > arr[i + 1])
                        {
                            stop = false;
                            break;
                        }
                        stop = true;
                    }
                    break;
                }
                case 3:         //Bubble Sort
                {
                    if (i == -1) {
                        i = 0;
                        j = 0;
                    }
                    if ((j == size - i - 1))
                    {
                        i++;
                        j = 0;
                    }
                    if (i == size - 1)
                    {
                        stop = true;
                        break;
                    }
                    if (arr[j] > arr[j + 1]) {
                        int temp = arr[j];
                        arr[j] = arr[j + 1];
                        arr[j + 1] = temp;
                    }
                    j++;
                    break;
                }
                case 4:         //Cocktail Sort
                {
                    if (i == -1) {
                        i = 0;
                        flipped = false;
                        flipped2 = false;
                        left = 0;
                        right = size - 1;
                        tempLeft = left;
                        tempRight = right;
                    }
                    if (flipped2)
                    {
                        if (tempRight < left)
                        {
                            if (!flipped)
                            {
                                stop = true;
                                break;
                            }
                            flipped = false;
                            flipped2 = false;
                            left++;
                            tempLeft = left;
                        }
                        if (arr[tempRight] > arr[tempRight + 1]) {
                            int temp = arr[tempRight];
                            arr[tempRight] = arr[tempRight + 1];
                            arr[tempRight + 1] = temp;
                            flipped = true;
                        }
                        tempRight--;
                    }
                    else
                    {
                        if (tempLeft == right - 1)
                        {
                            if (!flipped)
                            {
                                stop = true;
                                break;
                            }
                            flipped = false;
                            flipped2 = true;
                            right--;
                            tempRight = right - 1;
                        }
                        if (arr[tempLeft] > arr[tempLeft + 1]) {
                            int temp = arr[tempLeft];
                            arr[tempLeft] = arr[tempLeft + 1];
                            arr[tempLeft + 1] = temp;
                            flipped = true;
                        }
                        tempLeft++;
                    }
                    break;
                }
                case 5:         //Insertion Sort
                {
                    if (i == -1) {
                        i = 1;
                        k = arr[1];
                        j = 0;
                    }
                    if (j >= 0 && arr[j] > k) {
                        arr[j + 1] = arr[j];
                        j = j - 1;
                    }
                    else
                    {
                        arr[j + 1] = k;
                        i++;
                        k = arr[i];
                        j = i - 1;
                    }
                    if (i == size)
                    {
                        stop = true;
                    }
                    break;
                }
                case 6:         //Comb Sort
                {
                    if (i == -1) {
                        i = 0;
                        j = size - 1;
                        flipped = false;
                        flipped2 = true;
                    }
                    if (j == 1 && flipped2 == false)
                    {
                        stop = true;
                        break;
                    }
                    else
                    {
                        flipped2 = true;
                        if (i < size - j)
                        {
                            if (arr[i] > arr[i + j])
                            {
                                int temp = arr[i];
                                arr[i] = arr[j + i];
                                arr[j + i] = temp;
                                flipped = true;
                            }
                            i++;
                        }
                        else
                        {
                            i = 0;
                            j = (j * 10) / 13;
                            if (j < 1)
                            {
                                j = 1;
                            }
                            if (flipped == false)
                                flipped2 = false;
                            flipped = false;
                        }
                    }
                    break;
                }
                case 7:         //Shell Sort
                {
                    if (i == -1) {
                        i = size / 2;
                        j = i;
                        k = j;
                        tempLeft = arr[j];
                    }
                    if (i > 0)
                    {
                        if (j < size)
                        {
                            if (k >= i && arr[k - i] > tempLeft)
                            {
                                arr[k] = arr[k - i];
                                k -= i;
                            }
                            else {
                                arr[k] = tempLeft;
                                j += 1;
                                k = j;
                                tempLeft = arr[j];
                            }
                        }
                        else {
                            i /= 2;
                            j = i;
                            k = j;
                            tempLeft = arr[j];
                        }
                    }
                    else
                        stop = true;
                    break;
                }
                default:        //Merge Sort
                {
                    if (i == -1) {
                        i = 1;
                        j = 0;
                        k = 0;
                        l = 0;
                        left = 0;
                        right = 1;
                        tempLeft = 0;
                        tempRight = 1;
                    }
                    if (i < size)
                    {
                        if (i + j < size)
                        {
                            if (tempLeft <= left && tempRight <= right)
                            {
                                if (arr[tempLeft] <= arr[tempRight])
                                {

                                    tempArr[k] = arr[tempLeft];
                                    k++;
                                    tempLeft++;
                                }
                                else
                                {
                                    tempArr[k] = arr[tempRight];
                                    k++;
                                    tempRight++;
                                }
                            }
                            else if (tempLeft <= left)
                            {
                                tempArr[k] = arr[tempLeft];
                                k++;
                                tempLeft++;
                            }
                            else if (tempRight <= right)
                            {
                                tempArr[k] = arr[tempRight];
                                k++;
                                tempRight++;
                            }
                            else
                            {
                                j = right + 1;
                                left = j + i - 1;
                                right = left + i;
                                if (right >= size)
                                    right = size - 1;
                                tempLeft = j;
                                tempRight = j + i;
                                l = 0;
                            }
                        }
                        else if (k < size)
                        {
                            tempArr[k] = arr[j];
                            k++;
                            j++;
                        }
                        else if (l < size)
                        {
                            arr[l] = tempArr[l];
                            l++;
                        }
                        else
                        {
                            i = i * 2;
                            j = 0;
                            k = 0;
                            left = j + i - 1;
                            right = left + i;
                            if (right >= size)
                                right = size - 1;
                            tempLeft = j;
                            tempRight = j + i;
                            l = 0;
                        }
                    }
                    else
                        stop = true;
                    break;
                }
                }
            }

            ImGui::End();

            break;
        }
        default:        //Sudoku
        {
            ImGui::Begin("Sudoku");

            const ImVec2 p = ImGui::GetCursorScreenPos();

            const ImU32 colW = ImColor(ImVec4(1.0f, 1.0f, 1.0f, 255.0f)),
                colB = ImColor(ImVec4(0.0f, 0.0f, 0.0f, 255.0f));

            const ImU8  u8_Premin = 0,
                u8_Premax = 81,
                u8_Sizemin = 30,
                u8_Sizemax = 105;

            static int  Presz = 9,
                sz = 75,
                Row = 0,
                Col = 0,
                Board[9][9];

            static bool setup = true,
                stop = true,
                valid = false,
                Solid[9][9];

            float   x = p.x + 4.0f,
                y = p.y + 4.0f;

            static float    FixedX = 9,
                FixedY = 9;

            if (setup)                                  //Initialize Board
            {
                for (int i = 0; i < 9; i++)
                {
                    for (int j = 0; j < 9; j++)
                    {
                        Board[i][j] = 0;
                        Solid[i][j] = true;
                    }
                }
                setup = false;
            }

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + 9 * sz, y + 9 * sz), colW);
            ImGui::SetCursorScreenPos(ImVec2(x, y));
            if (ImGui::InvisibleButton(" ", ImVec2(9 * sz, 9 * sz))) {
                FixedY = (int)(io.MousePos.x - x) / sz;
                FixedX = (int)(io.MousePos.y - y) / sz;
            }
            for (int i = 0; i < 9; i++)                                         //Draw Numbers on Board
            {
                for (int j = 0; j < 9; j++)
                {
                    if ((int)FixedX == i && (int)FixedY == j)                    //Highlight selected point
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), ImColor(ImVec4(0.537f, 0.812f, 0.941f, 255.0f)));

                    switch (Board[i][j]) {
                    case 1: {
                        draw_list->AddLine(ImVec2(x + sz * .27, y + sz * .85), ImVec2(x + sz * .70, y + sz * .85), colB, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .50, y + sz * .85), ImVec2(x + sz * .50, y + sz * .15), colB, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .27, y + sz * .4), ImVec2(x + sz * .50, y + sz * .18), colB, sz / 16);
                        break;
                    }
                    case 2: {
                        draw_list->AddBezierCubic(ImVec2(x + sz * .27, y + sz * .4), ImVec2(x + sz * .60, y + sz * .0), ImVec2(x + sz * .90, y + sz * .4), ImVec2(x + sz * .29, y + sz * .86), colB, sz / 16, 12);
                        draw_list->AddLine(ImVec2(x + sz * .27, y + sz * .85), ImVec2(x + sz * .70, y + sz * .85), colB, sz / 16);

                        break;
                    }
                    case 3: {
                        draw_list->AddBezierCubic(ImVec2(x + sz * .27, y + sz * .25), ImVec2(x + sz * .75, y + sz * .05), ImVec2(x + sz * .75, y + sz * .55), ImVec2(x + sz * .27, y + sz * .55), colB, sz / 16, 12);
                        draw_list->AddBezierCubic(ImVec2(x + sz * .27, y + sz * .55), ImVec2(x + sz * .75, y + sz * .35), ImVec2(x + sz * .75, y + sz * 1.0), ImVec2(x + sz * .27, y + sz * .85), colB, sz / 16, 12);
                        break;
                    }
                    case 4: {
                        draw_list->AddLine(ImVec2(x + sz * .70, y + sz * .85), ImVec2(x + sz * .70, y + sz * .15), colB, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .30, y + sz * .50), ImVec2(x + sz * .30, y + sz * .15), colB, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .27, y + sz * .50), ImVec2(x + sz * .70, y + sz * .50), colB, sz / 16);
                        break;
                    }
                    case 5: {
                        draw_list->AddLine(ImVec2(x + sz * .27, y + sz * .15), ImVec2(x + sz * .65, y + sz * .15), colB, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .30, y + sz * .55), ImVec2(x + sz * .30, y + sz * .15), colB, sz / 16);
                        draw_list->AddBezierCubic(ImVec2(x + sz * .30, y + sz * .53), ImVec2(x + sz * .75, y + sz * .35), ImVec2(x + sz * .75, y + sz * 1.0), ImVec2(x + sz * .27, y + sz * .85), colB, sz / 16, 12);
                        break;
                    }
                    case 6: {
                        draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.65f), sz * 0.2f, colB, 40, sz / 16);

                        draw_list->AddBezierQuadratic(ImVec2(x + sz * .70, y + sz * .15), ImVec2(x + sz * .25, y + sz * .15), ImVec2(x + sz * .31, y + sz * .7), colB, sz / 16, 40);
                        break;
                    }
                    case 7: {

                        draw_list->AddLine(ImVec2(x + sz * .27, y + sz * .15), ImVec2(x + sz * .75, y + sz * .15), colB, sz / 16);
                        draw_list->AddLine(ImVec2(x + sz * .40, y + sz * .85), ImVec2(x + sz * .75, y + sz * .14), colB, sz / 16);
                        break;
                    }
                    case 8: {
                        draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.32f), sz * 0.15f, colB, 40, sz / 16);
                        draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.65f), sz * 0.2f, colB, 40, sz / 16);
                        break;
                    }
                    case 9: {
                        draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.35f), sz * 0.2f, colB, 40, sz / 16);

                        draw_list->AddBezierQuadratic(ImVec2(x + sz * .70, y + sz * .35), ImVec2(x + sz * .75, y + sz * .85), ImVec2(x + sz * .30, y + sz * .85), colB, sz / 16, 40);
                        break;
                    }
                    default:
                        break;
                    }
                    draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), colB, 0.0f, ImDrawFlags_None, sz / 16);
                    if (j % 3 == 0 && i % 3 == 0)
                        draw_list->AddRect(ImVec2(x, y), ImVec2(x + 3 * sz, y + 3 * sz), colB, 0.0f, ImDrawFlags_None, sz / 8);
                    x += sz;
                }
                x = p.x + 4;
                y += sz;
            }

            if (!stop)
            {
                if (!Solid[Row][Col]) {         //Back tracking Algorith
                    Board[Row][Col]++;          //Increases value in a square until a valid one is found
                    if (Board[Row][Col] == 10) {//If one is not found return to the last changeable square and repeat loop
                        Board[Row][Col] = 0;
                        Col--;
                        if (Col == -1) {
                            Col = 8;
                            Row -= 1;
                        }
                        while (Solid[Row][Col]) {
                            Col--;
                            if (Col == -1) {
                                Col = 8;
                                Row--;
                            }
                        }
                    }
                    else if (IsValid(Board, Row, Col, Board[Row][Col])) //If one is found continue to next square then repeat loop
                    {
                        Col++;
                    }
                }
                else
                    Col++;
            }

            if (Col == -1) {        //While backtrack if you reach first column return to previous row
                Col = 8;
                Row--;
            }
            else if (Col == 9)      //Once you reach last column go to next row
            {
                Col = 0;
                Row++;
            }

            if (Row == 9) {
                if (IsValid(Board, 8, 8, Board[8][8])) //Check if whole board is valid
                {
                    valid = true;
                    stop = true;
                }
                else
                {
                    Row = 8;
                    Col = 8;
                }
            }

            ImGui::End();
            ImGui::Begin("Settings");

            ImGui::Combo(" Projects", &project_type, program_names, IM_ARRAYSIZE(program_names), IM_ARRAYSIZE(program_names));
            ImGui::NewLine();

            ImGui::SliderScalar("Size", ImGuiDataType_U8, &sz, &u8_Sizemin, &u8_Sizemax, "%u");
            ImGui::SliderScalar("Prefilled Spaces", ImGuiDataType_U8, &Presz, &u8_Premin, &u8_Premax, "%u");
            ImGui::NewLine();

            if (ImGui::Button("Generate New Board"))
            {
                valid = false;
                stop = true;
                Row = 0;
                Col = 0;

                int temp[9] = { 1,2,3,4,5,6,7,8,9 };
                std::shuffle(temp, temp + (sizeof(temp) / sizeof(temp[0])), std::default_random_engine(time(NULL)));
                for (int i = 0; i < 9; i++)     //Initialize Board
                {
                    for (int j = 0; j < 9; j++)
                    {
                        Board[i][j] = 0;
                        Solid[i][j] = true;
                    }
                    Board[0][i] = temp[i];
                }
                for (int i = 1; i < 9; i++)     //Use Backtracking to make a valid board
                {
                    for (int j = 0; j < 9;)
                    {
                        Board[i][j]++;
                        if (Board[i][j] == 10) {
                            Board[i][j] = 0;
                            j--;
                            if (j == -1) {
                                j = 8;
                                i -= 1;
                            }
                        }
                        else if (IsValid(Board, i, j, Board[i][j]))
                        {
                            j++;
                        }
                    }
                }
                if (Presz == 81) {
                    valid = true;
                }
                else {
                    int temp2[81];
                    for (int i = 0; i < 81; i++)
                    {
                        temp2[i] = i;
                    }
                    std::shuffle(temp2, temp2 + (sizeof(temp2) / sizeof(temp2[0])), std::default_random_engine(time(NULL)));
                    for (int i = 0; i < 81 - Presz; i++)        //Randomize which square are revealed 
                    {
                        Board[temp2[i] % 9][temp2[i] / 9] = 0;
                        Solid[temp2[i] % 9][temp2[i] / 9] = false;
                    }
                }
            }
            else if (ImGui::Button("Start Solver"))
            {
                stop = false;
            }
            else if (ImGui::Button("Stop Solver"))
            {
                stop = true;
            }
            ImGui::NewLine();

            if (valid)
                ImGui::Text("Board is Valid");
            else
                ImGui::Text("Board is Invalid");

            for (ImGuiKey key = ImGuiKey_KeysData_OFFSET; key < ImGuiKey_COUNT; key = (ImGuiKey)(key + 1)) { //Read number inputs and place it in selected square
                if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key))
                    continue;
                if (536 <= key && 545 >= key && !Solid[(int)FixedX][(int)FixedY])
                    Board[(int)FixedX][(int)FixedY] = (key - 536);
                else if (600 <= key && 609 >= key && !Solid[(int)FixedX][(int)FixedY])
                    Board[(int)FixedX][(int)FixedY] = (key - 600);
            }
            ImGui::End();
            break;
        }
        }
        ImGui::End();
    }

}

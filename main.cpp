#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cstdio>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string NormalizeDate(const std::string &input)
{
    std::tm tm = {};
    std::istringstream ss(input);

    // Try multiple formats
    const char *formats[] = {
        "%m/%d/%y", // 04/20/26
        "%Y-%m-%d", // 2026-04-20
        "%d-%m-%Y", // 20-04-2026
        "%B %d, %Y" // April 20, 2026
    };

    for (auto fmt : formats)
    {
        ss.clear();
        ss.str(input);
        ss >> std::get_time(&tm, fmt);
        if (!ss.fail())
        {
            std::ostringstream out;
            out << std::put_time(&tm, "%m/%d/%y"); // normalize to MM/DD/YY
            return out.str();
        }
    }

    return input; // fallback if parsing fails
}

enum class EventType
{
    None,
    Party,
    Wedding,
    Birthday,
    Conference,
    Engagement
};
EventType selectedEvent = EventType::None;

////////////////////////////////////////////////////////////
// PUT ALL VARIABLES HERE
////////////////////////////////////////////////////////////
// ===== TASK TRACKING =====
std::vector<int> taskDone;
static std::vector<int> taskDaysLeft;

// ===== BUDGET =====
static float totalBudget = 10000.0f;
static float spentBudget = 0.0f;
// ---------------- BIRTHDAY ----------------

char birthdayTheme[100] = "";
float birthdayBudget = 0.0f;
char birthdayGuestList[200] = "";
char birthdayDateTime[50] = "";
char birthdayInvitations[100] = "";
char birthdayVenue[100] = "";
char birthdayCake[100] = "";
char birthdayDecorations[100] = "";
char birthdayFoodDrinks[100] = "";
char birthdayPlaylist[100] = "";
char birthdayRSVPs[100] = "";
char birthdaySetupVenue[100] = "";
char birthdayDecorateSpace[100] = "";
char birthdayFoodTable[100] = "";
char birthdayMusic[100] = "";
char birthdayPhotos[100] = "";
// ---------------- BIRTHDAY OPTIONS ----------------
enum class BirthdayOption
{
    None,
    Option1,
    Option2,
    Option3,
    Option4,
    Option5,
    Option6
};
BirthdayOption selectedBirthdayOption = BirthdayOption::None;

// ---------------- BIRTHDAY OPTION TASKS ----------------
// These match the categories from your table

std::vector<const char *> birthdayOption1Tasks = {
    "Venue", "Decorations", "Catering", "Cake",
    "Entertainment (DJ/Games)", "Activities/Games", "Miscellaneous"};

std::vector<const char *> birthdayOption2Tasks = {
    "Venue", "Decorations", "Catering", "Cake",
    "Entertainment (DJ/Games)", "Sound System", "Lighting",
    "Photographer", "Party Favors", "Miscellaneous"};

std::vector<const char *> birthdayOption3Tasks = {
    "Venue", "Decorations", "Catering", "Cake",
    "Entertainment (DJ/Games)", "Sound System", "Lighting",
    "Decor Upgrade (Theme Setup)", "Photographer", "Videography",
    "Party Favors", "Activities/Games"};

std::vector<const char *> birthdayOption4Tasks = {
    "Venue", "Decorations", "Catering", "Cake",
    "Entertainment (DJ/Games)", "Sound System", "Lighting",
    "Decor Upgrade (Theme Setup)", "Photographer", "Videography",
    "Party Favors", "Activities/Games"};

std::vector<const char *> birthdayOption5Tasks = {
    "Venue", "Decorations", "Catering", "Cake",
    "Entertainment (DJ/Games)", "Sound System", "Lighting",
    "Photographer", "Videography", "Party Favors", "Activities/Games"};

std::vector<const char *> birthdayOption6Tasks = {
    "Venue", "Decorations", "Catering", "Cake",
    "Entertainment (DJ/Games)", "Sound System", "Lighting",
    "Decor Upgrade (Theme Setup)", "Photographer", "Videography",
    "Party Favors"};

// ---------------- WEDDING ----------------

float weddingBudget = 0.0f;
char weddingDate[50] = "";
char weddingVenue[100] = "";
char weddingGuestList[200] = "";
char weddingPhotographer[100] = "";
char weddingVideographer[100] = "";
char weddingCatering[100] = "";
char weddingInvitations[100] = "";
char weddingOutfits[100] = "";
char weddingMakeupArtist[100] = "";
char weddingSeatingArrangement[100] = "";
char weddingFlowers[100] = "";
char weddingTransport[100] = "";
char weddingVendors[100] = "";
char weddingSchedule[100] = "";
char weddingSetupVenue[100] = "";
char weddingCeremony[100] = "";
char weddingTimeline[100] = "";

// ---------------- WEDDING OPTIONS ----------------
enum class WeddingOption
{
    None,
    Option1,
    Option2,
    Option3,
    Option4,
    Option5,
    Option6
};
WeddingOption selectedWeddingOption = WeddingOption::None;
// Option 1 (highest budget)
std::vector<const char *> weddingOption1Tasks = {
    "Suit", "Wedding Dress", "Bouquet", "Makeup/Coiffeur", "Venue",
    "Dance Floor", "Lights", "Decoration", "Photographer", "Catering",
    "Painter", "DJ", "Singer", "Cinematography", "Wedding Favors", "Transportation"};

// Option 2
std::vector<const char *> weddingOption2Tasks = {
    "Suit", "Wedding Dress", "Bouquet", "Makeup/Coiffeur", "Venue",
    "Dance Floor", "Lights", "Decoration", "Photographer", "Catering",
    "DJ", "Cinematography"};

// Option 3
std::vector<const char *> weddingOption3Tasks = {
    "Suit", "Wedding Dress", "Bouquet", "Makeup/Coiffeur", "Venue",
    "Dance Floor", "Lights", "Decoration", "Photographer", "Catering",
    "DJ", "Cinematography"};

// Option 4
std::vector<const char *> weddingOption4Tasks = {
    "Suit", "Wedding Dress", "Bouquet", "Makeup/Coiffeur", "Venue",
    "Dance Floor", "Lights", "Decoration", "Photographer", "Catering",
    "DJ", "Cinematography"};

// Option 5
std::vector<const char *> weddingOption5Tasks = {
    "Suit", "Wedding Dress", "Bouquet", "Makeup/Coiffeur", "Venue",
    "Dance Floor", "Lights", "Decoration", "Photographer", "Catering",
    "Soundsystem", "Cinematography"};

// Option 6 (lowest budget)
std::vector<const char *> weddingOption6Tasks = {
    "Suit", "Wedding Dress", "Bouquet",
    "Dance Floor", "Lights", "Decoration", "Photographer", "Catering",
    "Soundsystem"};
// ---------------- PARTY ----------------

char partyTheme[100] = "";
float partyBudget = 0.0f;
char partyGuestList[200] = "";
char partyDateTime[50] = "";
char partyInvitations[100] = "";
char partyFoodDrinks[100] = "";
char partySnacks[100] = "";
char partyDecorations[100] = "";
char partyPlaylist[100] = "";
char partySetupSpace[100] = "";
char partySeating[100] = "";
char partyMusicSystem[100] = "";
char partyWelcomeGuests[100] = "";
char partyServeFood[100] = "";
char partyPhotos[100] = "";
char partyCleanup[100] = "";
// ---------------- PARTY OPTIONS ----------------
enum class PartyOption
{
    None,
    Option1,
    Option2,
    Option3
};
PartyOption selectedPartyOption = PartyOption::None;

// ---------------- PARTY OPTION TASKS ----------------
// These match the categories from your table

std::vector<const char *> partyOption1Tasks = {
    "Venue", "Decor", "Lighting", "Photographer", "Videography",
    "Catering / Food & Drinks", "DJ / Soundsystem",
    "Entertainment (Singer / Show)", "Favors / Giveaways", "Transportation"};

std::vector<const char *> partyOption2Tasks = {
    "Venue", "Decor", "Lighting", "Photographer", "Videography",
    "Catering / Food & Drinks", "DJ / Soundsystem", "Favors / Giveaways"};

std::vector<const char *> partyOption3Tasks = {
    "Venue", "Decor", "Lighting", "Photographer", "Videography",
    "Catering / Food & Drinks", "DJ / Soundsystem"};

// ---------------- ENGAGEMENT ----------------

float engagementBudget = 0.0f;
char engagementDate[50] = "";
char engagementVenue[100] = "";
char engagementGuestList[200] = "";
char engagementInvitations[100] = "";
char engagementRings[100] = "";
char engagementDecorations[100] = "";
char engagementCatering[100] = "";
char engagementMusic[100] = "";
char engagementVendors[100] = "";
char engagementSchedule[100] = "";
// ---------------- ENGAGEMENT OPTIONS ----------------
enum class EngagementOption
{
    None,
    Option1,
    Option2,
    Option3,
    Option4,
    Option5
};
EngagementOption selectedEngagementOption = EngagementOption::None;

// ---------------- ENGAGEMENT OPTION TASKS ----------------
// Example task sets — you can adjust based on your table/requirements

std::vector<const char *> engagementOption1Tasks = {
    "Budget", "Date", "Venue", "Guest List", "Invitations",
    "Rings", "Decorations", "Catering", "Music", "Vendors", "Schedule"};

std::vector<const char *> engagementOption2Tasks = {
    "Budget", "Date", "Venue", "Guest List", "Invitations",
    "Decorations", "Catering", "Music", "Schedule"};

std::vector<const char *> engagementOption3Tasks = {
    "Budget", "Date", "Venue", "Guest List", "Invitations",
    "Catering", "Music"};

std::vector<const char *> engagementOption4Tasks = {
    "Budget", "Date", "Venue", "Guest List", "Invitations"};

std::vector<const char *> engagementOption5Tasks = {
    "Budget", "Date", "Venue"};

// ---------------- CONFERENCE ----------------

char conferenceTopic[100] = "";
float conferenceBudget = 0.0f;
char conferenceVenue[100] = "";
char conferenceAgenda[100] = "";
char conferenceSpeakers[200] = "";
char conferenceRegistration[100] = "";
char conferenceConfirmSpeakers[100] = "";
char conferenceEquipment[100] = "";
char conferenceWiFi[100] = "";
char conferencePromotion[100] = "";
char conferenceBadges[100] = "";
char conferenceStaffRoles[100] = "";
char conferenceCatering[100] = "";
char conferenceSchedule[100] = "";
char conferenceSetupVenue[100] = "";
char conferenceCheckIn[100] = "";
char conferenceSessions[100] = "";
char conferenceTechnicalIssues[100] = "";
// ---------------- CONFERENCE OPTIONS ----------------
enum class ConferenceOption
{
    None,
    Option1,
    Option2,
    Option3,
    Option4,
    Option5,
    Option6
};
ConferenceOption selectedConferenceOption = ConferenceOption::None;

// ---------------- CONFERENCE OPTION TASKS ----------------
// These match the categories from your table

std::vector<const char *> conferenceOption1Tasks = {
    "Venue", "Registration & Materials", "Catering (Coffee/Food)", "Tech (Screens/Projectors)"};

std::vector<const char *> conferenceOption2Tasks = {
    "Venue", "Stage Setup", "Audio System", "Registration & Materials",
    "Catering (Coffee/Food)", "Marketing & Promotion", "Speakers / Guest Fees", "Tech (Screens/Projectors)"};

std::vector<const char *> conferenceOption3Tasks = {
    "Venue", "Stage Setup", "Audio System", "Lighting", "Registration & Materials",
    "Catering (Coffee/Food)", "Marketing & Promotion", "Photography",
    "Speakers / Guest Fees", "Branding (Banners, Booths)", "Tech (Screens/Projectors)"};

std::vector<const char *> conferenceOption4Tasks = {
    "Venue", "Stage Setup", "Audio System", "Lighting", "Registration & Materials",
    "Catering (Coffee/Food)", "Marketing & Promotion", "Photography", "Videography",
    "Speakers / Guest Fees", "Branding (Banners, Booths)", "Security", "Tech (Screens/Projectors)", "Miscellaneous"};

std::vector<const char *> conferenceOption5Tasks = {
    "Venue", "Stage Setup", "Audio System", "Lighting", "Registration & Materials",
    "Catering (Coffee/Food)", "Marketing & Promotion", "Photography", "Videography",
    "Speakers / Guest Fees", "Branding (Banners, Booths)", "Security", "Tech (Screens/Projectors)", "Miscellaneous"};

std::vector<const char *> conferenceOption6Tasks = {
    "Venue", "Stage Setup", "Audio System", "Lighting", "Registration & Materials",
    "Catering (Coffee/Food)", "Marketing & Promotion", "Photography", "Videography",
    "Speakers / Guest Fees", "Branding (Banners, Booths)", "Security", "Tech (Screens/Projectors)", "Miscellaneous"};

std::vector<const char *> birthdayTasks = {
    "Choose theme",
    "Set budget",
    "Create guest list",
    "Pick date and time",
    "Send invitations",
    "Book venue",
    "Order cake",
    "Buy decorations",
    "Plan food and drinks",
    "Prepare playlist",
    "Confirm RSVPs",
    "Decorate space",
    "Play music",
    "Take photos"};

std::vector<const char *> weddingTasks = {
    "Set budget",
    "Choose wedding date",
    "Book venue",
    "Create guest list",
    "Hire photographer",
    "Hire videographer",
    "Book catering",
    "Send invitations",
    "Choose wedding outfits",
    "Book makeup artist",
    "Plan seating arrangement",
    "Order flowers",
    "Arrange transport",
    "Confirm vendors",
    "Final event schedule"};

std::vector<const char *> partyTasks = {
    "Choose theme",
    "Set budget",
    "Create guest list",
    "Pick date and time",
    "Send invitations",
    "Plan food and drinks",
    "Buy snacks and drinks",
    "Get decorations",
    "Prepare playlist",
    "Arrange seating",
    "Serve food and drinks",
    "Take photos",
    "Clean up after party"};

std::vector<const char *> conferenceTasks = {
    "Define topic",
    "Set budget",
    "Book venue",
    "Create agenda",
    "Invite speakers",
    "Open registration",
    "Arrange equipment",
    "Set up WiFi",
    "Promote event",
    "Print badges",
    "Assign staff roles",
    "Confirm catering",
    "Final schedule check",
    "Manage check-in"};
std::vector<const char *> engagementTasks = {
    "Set budget",
    "Choose date",
    "Book venue",
    "Create guest list",
    "Send invitations",
    "Order rings",
    "Plan decorations",
    "Arrange catering",
    "Prepare music",
    "Confirm vendors",
    "Finalize schedule"};
// ------------------------------------------------------------
// ------------------------------------------------------------

enum Screen
{
    SCREEN_LOGIN,
    SCREEN_CREATE_ACCOUNT,
    SCREEN_APP,
    SCREEN_EVENT_TYPE,
    SCREEN_EVENT_DETAILS,
    SCREEN_DASHBOARD

};
Screen currentScreen = SCREEN_LOGIN;

void RenderEventTypePage()
{
    ImGui::Begin("Create Event - Select Type");

    ImGui::Text("Choose your event type:");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));

    if (ImGui::Button("Wedding", ImVec2(200.0f, 60.0f)))
        selectedEvent = EventType::Wedding;

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a full wedding event");

    if (ImGui::Button("Engagement", ImVec2(200.0f, 60.0f)))
        selectedEvent = EventType::Engagement;

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Organize an engagement ceremony");

    if (ImGui::Button("Birthday Party", ImVec2(200.0f, 60.0f)))
        selectedEvent = EventType::Birthday;

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a birthday party");

    if (ImGui::Button("Party", ImVec2(200.0f, 60.0f)))
        selectedEvent = EventType::Party;

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a general party");
    if (ImGui::Button("Conference", ImVec2(200.0f, 60.0f)))
        selectedEvent = EventType::Conference;

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a corporate meeting or conference");
    std::vector<const char *> *tasks = nullptr;

    if (selectedEvent == EventType::Birthday)
        tasks = &birthdayTasks;

    else if (selectedEvent == EventType::Wedding)
        tasks = &weddingTasks;

    else if (selectedEvent == EventType::Party)
        tasks = &partyTasks;

    else if (selectedEvent == EventType::Engagement)
        tasks = &engagementTasks;

    else if (selectedEvent == EventType::Conference)
        tasks = &conferenceTasks;

    //  initialize vectors AFTER tasks is known
    if (tasks && taskDone.size() != tasks->size())
    {
        taskDone = std::vector<int>(tasks->size(), 0);
        taskDaysLeft = std::vector<int>(tasks->size(), 7);
        spentBudget = tasks->size() * 500;
    }

    // index is independent — just start from 0
    int index = 0;
    static char inputs[20][100] = {};

    if (tasks)
    {
        for (auto &task : *tasks)
        {
            ImGui::InputText(task, inputs[index],
                             IM_ARRAYSIZE(inputs[index]));
            index++; // counts position only
        }
    }
    // ===== INITIALIZE DASHBOARD DATA =====
    if (tasks && taskDone.size() != tasks->size())
    {
        taskDone = std::vector<int>(tasks->size(), 0);
        taskDaysLeft = std::vector<int>(tasks->size(), 7);

        spentBudget = tasks->size() * 500;
    }
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (selectedEvent != EventType::None)
    {
        ImGui::Text("Event selected!");
        ImGui::Spacing();

        if (ImGui::Button("Next ->", ImVec2(140.0f, 40.0f)))
        {
            currentScreen = SCREEN_EVENT_DETAILS;
        }
    }

    if (ImGui::Button("Back to Login", ImVec2(160.0f, 35.0f)))
    {
        currentScreen = SCREEN_LOGIN;
    }

    ImGui::PopStyleVar(2);

    ImGui::End();
}

// ------------------------------------------------------------
// ACCOUNT FUNCTIONS
// ------------------------------------------------------------

bool accountExists(const char *username, const char *password)
{
    std::ifstream file("accounts.txt");

    if (!file.is_open())
        return false;

    std::string line;

    while (std::getline(file, line))
    {
        size_t commaPos = line.find(',');

        if (commaPos == std::string::npos)
            continue;

        std::string user = line.substr(0, commaPos);
        std::string pass = line.substr(commaPos + 1);

        if (user == username && pass == password)
        {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

void saveAccount(const char *username, const char *password)
{
    std::ofstream file("accounts.txt", std::ios::app);

    if (!file.is_open())
        return;

    // Save using comma separator so spaces work in usernames
    file << username << "," << password << std::endl;

    file.close();
}

// ------------------------------------------------------------
// Load background texture
// ------------------------------------------------------------

GLuint LoadTexture(const char *filename)
{
    int width, height, channels;

    unsigned char *data =
        stbi_load(filename, &width, &height,
                  &channels, 4);

    if (!data)
    {
        printf("Failed to load image: %s\n", filename);
        return 0;
    }

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR);

    stbi_image_free(data);

    return texture;
}

// ------------------------------------------------------------
// Pink theme styling
// ------------------------------------------------------------

void SetPinkTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowRounding = 18.0f;
    style.FrameRounding = 12.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 10.0f;

    ImVec4 *colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 0.93f, 0.96f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(1.0f, 0.55f, 0.75f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.65f, 0.82f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.45f, 0.70f, 1.0f);

    colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 0.85f, 0.90f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 0.80f, 0.88f, 1.0f);

    colors[ImGuiCol_Text] = ImVec4(0.55f, 0.0f, 0.30f, 1.0f);
}
void RenderEventDetailsPage()
{
    ImGui::Begin("Event Details");

    // ================= PARTY =================
    if (selectedEvent == EventType::Party)
    {
        ImGui::Text("Party Event Details");
        ImGui::Separator();

        if (selectedPartyOption == PartyOption::None)
        {
            ImGui::Text("Choose Party Option:");
            ImGui::Spacing();

            if (ImGui::Button("Option 1 (Premium)"))
                selectedPartyOption = PartyOption::Option1;
            if (ImGui::Button("Option 2"))
                selectedPartyOption = PartyOption::Option2;
            if (ImGui::Button("Option 3 (Low Budget)"))
                selectedPartyOption = PartyOption::Option3;
        }
        else
        {
            // Show input fields based on chosen option
            std::vector<const char *> *tasks = nullptr;
            switch (selectedPartyOption)
            {
            case PartyOption::Option1:
                tasks = &partyOption1Tasks;
                break;
            case PartyOption::Option2:
                tasks = &partyOption2Tasks;
                break;
            case PartyOption::Option3:
                tasks = &partyOption3Tasks;
                break;
            default:
                break;
            }

            static char inputs[50][100] = {};

            int index = 0;

            for (auto &task : *tasks)
            {
                if (index >= 50)
                    break;

                ImGui::InputText(
                    task,
                    inputs[index],
                    IM_ARRAYSIZE(inputs[index]));

                index++;
            }
            ImGui::Spacing();
            if (ImGui::Button("Change Option"))
            {
                selectedPartyOption = PartyOption::None; // reset so user can re-choose
            }
        }
    }

    // ================= WEDDING =================
    if (selectedEvent == EventType::Wedding)
    {
        ImGui::Text("Wedding Event Details");
        ImGui::Separator();

        // Step 1: Let user choose an option first
        if (selectedWeddingOption == WeddingOption::None)
        {
            ImGui::Text("Choose Wedding Option:");
            ImGui::Spacing();

            if (ImGui::Button("Option 1 (Highest Budget)"))
                selectedWeddingOption = WeddingOption::Option1;
            if (ImGui::Button("Option 2"))
                selectedWeddingOption = WeddingOption::Option2;
            if (ImGui::Button("Option 3"))
                selectedWeddingOption = WeddingOption::Option3;
            if (ImGui::Button("Option 4"))
                selectedWeddingOption = WeddingOption::Option4;
            if (ImGui::Button("Option 5"))
                selectedWeddingOption = WeddingOption::Option5;
            if (ImGui::Button("Option 6 (Lowest Budget)"))
                selectedWeddingOption = WeddingOption::Option6;
        }
        else
        {
            // Step 2: Show input fields based on chosen option
            std::vector<const char *> *tasks = nullptr;
            switch (selectedWeddingOption)
            {
            case WeddingOption::Option1:
                tasks = &weddingOption1Tasks;
                break;
            case WeddingOption::Option2:
                tasks = &weddingOption2Tasks;
                break;
            case WeddingOption::Option3:
                tasks = &weddingOption3Tasks;
                break;
            case WeddingOption::Option4:
                tasks = &weddingOption4Tasks;
                break;
            case WeddingOption::Option5:
                tasks = &weddingOption5Tasks;
                break;
            case WeddingOption::Option6:
                tasks = &weddingOption6Tasks;
                break;
            default:
                break;
            }

            if (tasks)
            {
                ImGui::Text("Fill in details for your chosen option:");
                ImGui::Separator();

                // Each task gets its own input field
                static char inputs[50][100]; // 50 tasks max, each with 100 chars
                int index = 0;
                for (auto &task : *tasks)
                {
                    ImGui::InputText(task, inputs[index], IM_ARRAYSIZE(inputs[index]));
                    index++;
                }
            }

            ImGui::Spacing();
            if (ImGui::Button("Change Option"))
            {
                selectedWeddingOption = WeddingOption::None; // reset so user can re-choose
            }
        }
    }

    // ================= BIRTHDAY =================
    if (selectedEvent == EventType::Birthday)
    {
        ImGui::Text("Birthday Event Details");
        ImGui::Separator();

        if (selectedBirthdayOption == BirthdayOption::None)
        {
            ImGui::Text("Choose Birthday Option:");
            ImGui::Spacing();

            if (ImGui::Button("Option 1 (Basic)"))
                selectedBirthdayOption = BirthdayOption::Option1;
            if (ImGui::Button("Option 2 (Casual)"))
                selectedBirthdayOption = BirthdayOption::Option2;
            if (ImGui::Button("Option 3 (Balanced)"))
                selectedBirthdayOption = BirthdayOption::Option3;
            if (ImGui::Button("Option 4 (Aesthetic)"))
                selectedBirthdayOption = BirthdayOption::Option4;
            if (ImGui::Button("Option 5 (Entertainment-heavy)"))
                selectedBirthdayOption = BirthdayOption::Option5;
            if (ImGui::Button("Option 6 (Luxury)"))
                selectedBirthdayOption = BirthdayOption::Option6;
        }
        else
        {
            // Show input fields based on chosen option
            std::vector<const char *> *tasks = nullptr;
            switch (selectedBirthdayOption)
            {
            case BirthdayOption::Option1:
                tasks = &birthdayOption1Tasks;
                break;
            case BirthdayOption::Option2:
                tasks = &birthdayOption2Tasks;
                break;
            case BirthdayOption::Option3:
                tasks = &birthdayOption3Tasks;
                break;
            case BirthdayOption::Option4:
                tasks = &birthdayOption4Tasks;
                break;
            case BirthdayOption::Option5:
                tasks = &birthdayOption5Tasks;
                break;
            case BirthdayOption::Option6:
                tasks = &birthdayOption6Tasks;
                break;
            default:
                break;
            }

            if (tasks)
            {
                ImGui::Text("Fill in details for your chosen option:");
                ImGui::Separator();

                static char inputs[50][100]; // up to 50 tasks, each with 100 chars
                int index = 0;
                for (auto &task : *tasks)
                {
                    ImGui::InputText(task, inputs[index], IM_ARRAYSIZE(inputs[index]));
                    index++;
                }
            }

            ImGui::Spacing();
            if (ImGui::Button("Change Option"))
            {
                selectedBirthdayOption = BirthdayOption::None; // reset so user can re-choose
            }
        }
    }

    // ================= ENGAGEMENT =================
    if (selectedEvent == EventType::Engagement)
    {
        ImGui::Text("Engagement Event Details");
        ImGui::Separator();

        if (selectedEngagementOption == EngagementOption::None)
        {
            ImGui::Text("Choose Engagement Option:");
            ImGui::Spacing();

            if (ImGui::Button("Option 1 (Premium)"))
                selectedEngagementOption = EngagementOption::Option1;
            if (ImGui::Button("Option 2"))
                selectedEngagementOption = EngagementOption::Option2;
            if (ImGui::Button("Option 3"))
                selectedEngagementOption = EngagementOption::Option3;
            if (ImGui::Button("Option 4"))
                selectedEngagementOption = EngagementOption::Option4;
            if (ImGui::Button("Option 5 (Low Budget)"))
                selectedEngagementOption = EngagementOption::Option5;
        }
        else
        {
            // Show input fields based on chosen option
            std::vector<const char *> *tasks = nullptr;
            switch (selectedEngagementOption)
            {
            case EngagementOption::Option1:
                tasks = &engagementOption1Tasks;
                break;
            case EngagementOption::Option2:
                tasks = &engagementOption2Tasks;
                break;
            case EngagementOption::Option3:
                tasks = &engagementOption3Tasks;
                break;
            case EngagementOption::Option4:
                tasks = &engagementOption4Tasks;
                break;
            case EngagementOption::Option5:
                tasks = &engagementOption5Tasks;
                break;
            default:
                break;
            }

            if (tasks)
            {
                ImGui::Text("Fill in details for your chosen option:");
                ImGui::Separator();

                static char inputs[50][100]; // up to 50 tasks, each 100 chars
                int index = 0;
                for (auto &task : *tasks)
                {
                    ImGui::InputText(task, inputs[index], IM_ARRAYSIZE(inputs[index]));
                    index++;
                }
            }

            ImGui::Spacing();
            if (ImGui::Button("Change Option"))
            {
                selectedEngagementOption = EngagementOption::None; // reset so user can re-choose
            }
        }
    }

    // ================= CONFERENCE =================
    if (selectedEvent == EventType::Conference)
    {
        ImGui::Text("Conference Event Details");
        ImGui::Separator();

        if (selectedConferenceOption == ConferenceOption::None)
        {
            ImGui::Text("Choose Conference Option:");
            ImGui::Spacing();

            if (ImGui::Button("Option 1 (Minimal)"))
                selectedConferenceOption = ConferenceOption::Option1;
            if (ImGui::Button("Option 2 (Basic)"))
                selectedConferenceOption = ConferenceOption::Option2;
            if (ImGui::Button("Option 3 (Standard)"))
                selectedConferenceOption = ConferenceOption::Option3;
            if (ImGui::Button("Option 4 (Professional)"))
                selectedConferenceOption = ConferenceOption::Option4;
            if (ImGui::Button("Option 5 (Premium)"))
                selectedConferenceOption = ConferenceOption::Option5;
            if (ImGui::Button("Option 6 (Elite Summit)"))
                selectedConferenceOption = ConferenceOption::Option6;
        }
        else
        {
            // Show input fields based on chosen option
            std::vector<const char *> *tasks = nullptr;
            switch (selectedConferenceOption)
            {
            case ConferenceOption::Option1:
                tasks = &conferenceOption1Tasks;
                break;
            case ConferenceOption::Option2:
                tasks = &conferenceOption2Tasks;
                break;
            case ConferenceOption::Option3:
                tasks = &conferenceOption3Tasks;
                break;
            case ConferenceOption::Option4:
                tasks = &conferenceOption4Tasks;
                break;
            case ConferenceOption::Option5:
                tasks = &conferenceOption5Tasks;
                break;
            case ConferenceOption::Option6:
                tasks = &conferenceOption6Tasks;
                break;
            default:
                break;
            }

            if (tasks)
            {
                ImGui::Text("Fill in details for your chosen option:");
                ImGui::Separator();

                static char inputs[50][100]; // up to 50 tasks, each with 100 chars
                int index = 0;
                for (auto &task : *tasks)
                {
                    ImGui::InputText(task, inputs[index], IM_ARRAYSIZE(inputs[index]));
                    index++;
                }
            }

            ImGui::Spacing();
            if (ImGui::Button("Change Option"))
            {
                selectedConferenceOption = ConferenceOption::None; // reset so user can re-choose
            }
        }
    }

    ImGui::Spacing();
    if (ImGui::Button("Go to Dashboard"))
    {
        currentScreen = SCREEN_DASHBOARD;
    }

    if (ImGui::Button("Back"))
    {
        currentScreen = SCREEN_EVENT_TYPE;
    }

    ImGui::End();
}
void RenderDashboard()
{
    ImGui::Begin("Dashboard");

    int completed = 0;

    ImGui::Text("To-Do List");
    ImGui::Separator();

    if (!taskDone.empty())
    {
        for (int i = 0; i < taskDone.size(); i++)
        {
            ImGui::Checkbox(
                std::to_string(i + 1).c_str(),
                (bool*)&taskDone[i]);
            if (taskDone[i])
                completed++;
        }
    }
    else
    {
        ImGui::Text("No tasks yet");
    }

    float progress =
        taskDone.empty()
            ? 0.0f
            : (float)completed / taskDone.size();

    ImGui::Separator();

    ImGui::Text("Progress");

    ImGui::ProgressBar(
        progress,
        ImVec2(0.0f, 0.0f));

    ImGui::Text(
        "%.0f%% completed",
        progress * 100);

    ImGui::Separator();

    ImGui::Text("Budget");

    ImGui::Text(
        "Spent: %.0f / %.0f",
        spentBudget,
        totalBudget);

    if (spentBudget <= totalBudget)
        ImGui::Text("On track");
    else
        ImGui::Text("Over budget");

    ImGui::Separator();

    ImGui::Text("Stress Level");

    if (progress < 0.3f)
        ImGui::Text("High stress — start one task");
    else if (progress < 0.7f)
        ImGui::Text("Moderate stress — keep going");
    else
        ImGui::Text("Low stress — great job");

    ImGui::End();
}
// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------

int main()
{

    if (!glfwInit())
        return 1;

    const char *glsl_version = "#version 150";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(
        1920,
        1080,
        "Eventopia",
        NULL,
        NULL);
    if (window == NULL)
    {
        printf("Window creation failed\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    SetPinkTheme();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        float window_width = io.DisplaySize.x;
        float window_height = io.DisplaySize.y;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);

        // ================= LOGIN =================

        if (currentScreen == SCREEN_LOGIN)
        {
            ImGui::Begin("Welcome to Eventopia", NULL,
                         ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoTitleBar);

            static char username[50] = "";
            static char password[50] = "";

            static std::string loginMessage = "";

            float field_width = 260;

            ImGui::Text("Login to continue");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            ImGui::PushItemWidth(field_width);
            ImGui::InputText("Username", username, 50);
            ImGui::PopItemWidth();

            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            ImGui::PushItemWidth(field_width);
            ImGui::InputText(
                "Password",
                password,
                50,
                ImGuiInputTextFlags_Password);
            ImGui::PopItemWidth();

            ImGui::Spacing();

            bool canLogin =
                strlen(username) > 0 &&
                strlen(password) > 0;

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            if (!canLogin)
                ImGui::BeginDisabled();

            if (ImGui::Button(
                    "Login",
                    ImVec2(field_width, 50)))
            {
                if (accountExists(username, password))
                {
                    loginMessage = "Login successful!";
                    currentScreen = SCREEN_APP;
                }
                else
                {
                    loginMessage =
                        "Incorrect username or password.";
                }
            }

            if (!canLogin)
                ImGui::EndDisabled();

            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            if (ImGui::Button(
                    "Create Account",
                    ImVec2(field_width, 50)))
            {
                currentScreen = SCREEN_CREATE_ACCOUNT;
            }

            ImGui::Spacing();

            ImGui::Text("%s", loginMessage.c_str());

            ImGui::End();
        }

        // ================= CREATE ACCOUNT =================

        if (currentScreen == SCREEN_CREATE_ACCOUNT)
        {
            ImGui::Begin("Create Account", NULL,
                         ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoTitleBar);

            static char newUsername[50] = "";
            static char newPassword[50] = "";

            static std::string message = "";

            float field_width = 260;

            ImGui::Text("Create your account");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            ImGui::PushItemWidth(field_width);
            ImGui::InputText(
                "New Username",
                newUsername,
                50);
            ImGui::PopItemWidth();

            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            ImGui::PushItemWidth(field_width);
            ImGui::InputText(
                "New Password",
                newPassword,
                50,
                ImGuiInputTextFlags_Password);
            ImGui::PopItemWidth();

            ImGui::Spacing();

            bool canCreate =
                strlen(newUsername) > 0 &&
                strlen(newPassword) > 0;

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            if (!canCreate)
                ImGui::BeginDisabled();

            if (ImGui::Button(
                    "Create Account",
                    ImVec2(field_width, 50)))
            {
                saveAccount(newUsername, newPassword);

                message =
                    "Account created successfully!";

                strcpy(newUsername, "");
                strcpy(newPassword, "");
            }

            if (!canCreate)
                ImGui::EndDisabled();

            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            if (ImGui::Button(
                    "Back to Login",
                    ImVec2(field_width, 50)))
            {
                currentScreen = SCREEN_LOGIN;
            }

            ImGui::Spacing();

            ImGui::Text("%s", message.c_str());

            ImGui::End();
        }

        // ================= APP =================

        if (currentScreen == SCREEN_APP)
        {
            ImGui::Begin("Event Creator", NULL,
                         ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoTitleBar);

            ImGui::Text(
                "Welcome to the Event Planning System!");

            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - 260) * 0.5f);

            if (ImGui::Button(
                    "Create Event",
                    ImVec2(260, 60)))
            {
                currentScreen = SCREEN_EVENT_TYPE;
            }

            ImGui::End();
        }
        else if (currentScreen == SCREEN_EVENT_TYPE)
        {
            RenderEventTypePage();
        }
        else if (currentScreen == SCREEN_EVENT_DETAILS)
        {
            RenderEventDetailsPage();
        }
        else if (currentScreen == SCREEN_DASHBOARD)
        {
            RenderDashboard();
        }

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "calendar.h"
#include <cstdio>
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <filesystem>

// Forward declarations
void syncAndSaveCurrentEvent();

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

int DaysUntilEvent(const char *dateText)
{
    std::tm tm = {};
    std::istringstream ss(dateText);
    const char *formats[] = {
        "%m/%d/%y",
        "%Y-%m-%d",
        "%d-%m-%Y",
        "%B %d, %Y"
    };

    bool parsed = false;
    for (auto fmt : formats)
    {
        ss.clear();
        ss.str(dateText);
        ss >> std::get_time(&tm, fmt);
        if (!ss.fail())
        {
            parsed = true;
            break;
        }
    }

    if (!parsed)
        return -1;

    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    tm.tm_isdst = -1;

    std::time_t eventTime = std::mktime(&tm);
    if (eventTime == -1)
        return -1;

    std::time_t now = std::time(nullptr);
    std::tm *nowTm = std::localtime(&now);
    nowTm->tm_hour = 0;
    nowTm->tm_min = 0;
    nowTm->tm_sec = 0;
    nowTm->tm_isdst = -1;
    std::time_t today = std::mktime(nowTm);

    double diff = std::difftime(eventTime, today);
    return (int)std::ceil(diff / (60.0 * 60.0 * 24.0));
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

struct Event {
    std::string name;
    std::string date;
    std::string location;
    float budget;
    std::string guestList;
    std::vector<std::string> tasks;
    std::vector<bool> taskCompleted;
    std::string notes;
    EventType type;
    int selectedOption;
};

std::vector<Event> savedEvents;
Event *currentEvent = nullptr;
std::vector<std::string> currentEventTasks;

////////////////////////////////////////////////////////////
// PUT ALL VARIABLES HERE
////////////////////////////////////////////////////////////
// ===== CALENDAR STATE =====
CalendarState calendarState;

// ===== TASK TRACKING =====
std::vector<bool> taskDone;
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
    Option3,
    Option4,
    Option5,
    Option6
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

std::vector<const char *> partyOption4Tasks = {
    "Venue", "Decor", "Lighting", "Photographer", "Videography",
    "Catering / Food & Drinks", "DJ / Soundsystem"};

std::vector<const char *> partyOption5Tasks = {
    "Venue", "Decor", "Lighting", "Photographer", "Videography",
    "Catering / Food & Drinks", "DJ / Soundsystem"};

std::vector<const char *> partyOption6Tasks = {
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
// Example task sets - you can adjust based on your table/requirements

std::vector<const char *> engagementOption1Tasks = {
    "Date", "Venue", "Guest List", "Invitations",
    "Rings", "Decorations", "Catering", "Music", "Vendors", "Schedule"};

std::vector<const char *> engagementOption2Tasks = {
    "Date", "Venue", "Guest List", "Invitations",
    "Decorations", "Catering", "Music", "Schedule"};

std::vector<const char *> engagementOption3Tasks = {
    "Date", "Venue", "Guest List", "Invitations",
    "Catering", "Music"};

std::vector<const char *> engagementOption4Tasks = {
    "Date", "Venue", "Guest List", "Invitations"};

std::vector<const char *> engagementOption5Tasks = {
    "Date", "Venue"};

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

// ===== BUDGET RANGES IN EGP =====
float weddingMinBudget = 0.0f;
float weddingMaxBudget = 0.0f;
float birthdayMinBudget = 0.0f;
float birthdayMaxBudget = 0.0f;
float engagementMinBudget = 0.0f;
float engagementMaxBudget = 0.0f;
float partyMinBudget = 0.0f;
float partyMaxBudget = 0.0f;
float conferenceMinBudget = 0.0f;
float conferenceMaxBudget = 0.0f;
std::string budgetRangeText = "";

// Shared event detail fields
const std::vector<const char *> *currentChecklistTasks = nullptr;
std::vector<std::string> *currentDynamicTasks = nullptr;
char selectedEventDate[50] = "";
char selectedEventLocation[100] = "";
char selectedEventGuests[50] = "";
char selectedEventName[100] = "";
std::string selectedOptionName = "";

void UpdateChecklistTasks(const std::vector<const char *> &tasks)
{
    currentChecklistTasks = &tasks;
    currentDynamicTasks = nullptr;
    taskDone = std::vector<bool>(tasks.size(), false);
    taskDaysLeft = std::vector<int>(tasks.size(), 7);
}

void UpdateDynamicTasks(std::vector<std::string> &tasks)
{
    currentDynamicTasks = &tasks;
    currentChecklistTasks = nullptr;
    taskDone = std::vector<bool>(tasks.size(), false);
    taskDaysLeft = std::vector<int>(tasks.size(), 7);
}

void ResetEventOptionState()
{
    selectedBirthdayOption = BirthdayOption::None;
    selectedWeddingOption = WeddingOption::None;
    selectedPartyOption = PartyOption::None;
    selectedConferenceOption = ConferenceOption::None;
    selectedEngagementOption = EngagementOption::None;
    birthdayMinBudget = birthdayMaxBudget = 0.0f;
    weddingMinBudget = weddingMaxBudget = 0.0f;
    partyMinBudget = partyMaxBudget = 0.0f;
    conferenceMinBudget = conferenceMaxBudget = 0.0f;
    engagementMinBudget = engagementMaxBudget = 0.0f;
    budgetRangeText.clear();
    selectedOptionName.clear();
}

void SetOptionBudgetRange()
{
    if (selectedEvent == EventType::Birthday && selectedBirthdayOption != BirthdayOption::None)
    {
        switch (selectedBirthdayOption)
        {
            case BirthdayOption::Option1:
                birthdayMinBudget = 7000.0f;
                birthdayMaxBudget = 9000.0f;
                budgetRangeText = "Recommended budget: 7,000 - 9,000 EGP";
                break;
            case BirthdayOption::Option2:
                birthdayMinBudget = 11000.0f;
                birthdayMaxBudget = 13000.0f;
                budgetRangeText = "Recommended budget: 11,000 - 13,000 EGP";
                break;
            case BirthdayOption::Option3:
                birthdayMinBudget = 16000.0f;
                birthdayMaxBudget = 20000.0f;
                budgetRangeText = "Recommended budget: 16,000 - 20,000 EGP";
                break;
            case BirthdayOption::Option4:
                birthdayMinBudget = 25000.0f;
                birthdayMaxBudget = 31000.0f;
                budgetRangeText = "Recommended budget: 25,000 - 31,000 EGP";
                break;
            case BirthdayOption::Option5:
                birthdayMinBudget = 39000.0f;
                birthdayMaxBudget = 45000.0f;
                budgetRangeText = "Recommended budget: 39,000 - 45,000 EGP";
                break;
            case BirthdayOption::Option6:
                birthdayMinBudget = 54000.0f;
                birthdayMaxBudget = 66000.0f;
                budgetRangeText = "Recommended budget: 54,000 - 66,000 EGP";
                break;
            default:
                break;
        }
    }
    else if (selectedEvent == EventType::Wedding && selectedWeddingOption != WeddingOption::None)
    {
        switch (selectedWeddingOption)
        {
            case WeddingOption::Option1:
                weddingMinBudget = 9000000.0f;
                weddingMaxBudget = 11000000.0f;
                budgetRangeText = "Recommended budget: 9,000,000 - 11,000,000 EGP";
                break;
            case WeddingOption::Option2:
                weddingMinBudget = 5000000.0f;
                weddingMaxBudget = 6000000.0f;
                budgetRangeText = "Recommended budget: 5,000,000 - 6,000,000 EGP";
                break;
            case WeddingOption::Option3:
                weddingMinBudget = 2700000.0f;
                weddingMaxBudget = 3300000.0f;
                budgetRangeText = "Recommended budget: 2,700,000 - 3,300,000 EGP";
                break;
            case WeddingOption::Option4:
                weddingMinBudget = 1350000.0f;
                weddingMaxBudget = 1650000.0f;
                budgetRangeText = "Recommended budget: 1,350,000 - 1,650,000 EGP";
                break;
            case WeddingOption::Option5:
                weddingMinBudget = 720000.0f;
                weddingMaxBudget = 880000.0f;
                budgetRangeText = "Recommended budget: 720,000 - 880,000 EGP";
                break;
            case WeddingOption::Option6:
                weddingMinBudget = 135000.0f;
                weddingMaxBudget = 165000.0f;
                budgetRangeText = "Recommended budget: 135,000 - 165,000 EGP";
                break;
            default:
                break;
        }
    }
    else if (selectedEvent == EventType::Party && selectedPartyOption != PartyOption::None)
    {
        switch (selectedPartyOption)
        {
            case PartyOption::Option1:
                partyMinBudget = 6000.0f;
                partyMaxBudget = 10000.0f;
                budgetRangeText = "Recommended budget: 6,000 - 10,000 EGP";
                break;
            case PartyOption::Option2:
                partyMinBudget = 10000.0f;
                partyMaxBudget = 14000.0f;
                budgetRangeText = "Recommended budget: 10,000 - 14,000 EGP";
                break;
            case PartyOption::Option3:
                partyMinBudget = 15000.0f;
                partyMaxBudget = 20000.0f;
                budgetRangeText = "Recommended budget: 15,000 - 20,000 EGP";
                break;
            case PartyOption::Option4:
                partyMinBudget = 24000.0f;
                partyMaxBudget = 32000.0f;
                budgetRangeText = "Recommended budget: 24,000 - 32,000 EGP";
                break;
            case PartyOption::Option5:
                partyMinBudget = 38000.0f;
                partyMaxBudget = 46000.0f;
                budgetRangeText = "Recommended budget: 38,000 - 46,000 EGP";
                break;
            default:
                break;
        }
    }
    else if (selectedEvent == EventType::Conference && selectedConferenceOption != ConferenceOption::None)
    {
        switch (selectedConferenceOption)
        {
            case ConferenceOption::Option1:
                conferenceMinBudget = 5000.0f;
                conferenceMaxBudget = 10000.0f;
                budgetRangeText = "Recommended budget: 5,000 - 10,000 EGP";
                break;
            case ConferenceOption::Option2:
                conferenceMinBudget = 10000.0f;
                conferenceMaxBudget = 20000.0f;
                budgetRangeText = "Recommended budget: 10,000 - 20,000 EGP";
                break;
            case ConferenceOption::Option3:
                conferenceMinBudget = 20000.0f;
                conferenceMaxBudget = 40000.0f;
                budgetRangeText = "Recommended budget: 20,000 - 40,000 EGP";
                break;
            case ConferenceOption::Option4:
                conferenceMinBudget = 40000.0f;
                conferenceMaxBudget = 70000.0f;
                budgetRangeText = "Recommended budget: 40,000 - 70,000 EGP";
                break;
            case ConferenceOption::Option5:
                conferenceMinBudget = 70000.0f;
                conferenceMaxBudget = 100000.0f;
                budgetRangeText = "Recommended budget: 70,000 - 100,000 EGP";
                break;
            case ConferenceOption::Option6:
                conferenceMinBudget = 100000.0f;
                conferenceMaxBudget = 200000.0f;
                budgetRangeText = "Recommended budget: 100,000 - 200,000 EGP";
                break;
            default:
                break;
        }
    }
    else if (selectedEvent == EventType::Engagement && selectedEngagementOption != EngagementOption::None)
    {
        switch (selectedEngagementOption)
        {
            case EngagementOption::Option1:
                engagementMinBudget = 1800000.0f;
                engagementMaxBudget = 2200000.0f;
                budgetRangeText = "Recommended budget: 1,800,000 - 2,200,000 EGP";
                break;
            case EngagementOption::Option2:
                engagementMinBudget = 900000.0f;
                engagementMaxBudget = 1100000.0f;
                budgetRangeText = "Recommended budget: 900,000 - 1,100,000 EGP";
                break;
            case EngagementOption::Option3:
                engagementMinBudget = 540000.0f;
                engagementMaxBudget = 660000.0f;
                budgetRangeText = "Recommended budget: 540,000 - 660,000 EGP";
                break;
            case EngagementOption::Option4:
                engagementMinBudget = 270000.0f;
                engagementMaxBudget = 330000.0f;
                budgetRangeText = "Recommended budget: 270,000 - 330,000 EGP";
                break;
            case EngagementOption::Option5:
                engagementMinBudget = 72000.0f;
                engagementMaxBudget = 88000.0f;
                budgetRangeText = "Recommended budget: 72,000 - 88,000 EGP";
                break;
            default:
                break;
        }
    }
}

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
// ------------------------------------------------------------
// ------------------------------------------------------------

enum Screen
{
    SCREEN_LOGIN,
    SCREEN_CREATE_ACCOUNT,
    SCREEN_APP,
    SCREEN_MAIN_MENU,
    SCREEN_EVENT_SELECTION,
    SCREEN_EVENT_OPTIONS,
    SCREEN_EVENT_TYPE,
    SCREEN_EVENT_DETAILS,
    SCREEN_DASHBOARD,
    SCREEN_CALENDAR,
    SCREEN_BUDGET_SYSTEM,
    SCREEN_EVENT_LIST,
    SCREEN_EVENT_BASIC_DETAILS
};
Screen currentScreen = SCREEN_LOGIN;
GLuint backgroundTexture = 0;

// ===== BUDGET ALLOCATION PERCENTAGES =====
// Birthday Party Budget Breakdown (by percentage for each option)
struct BirthdayBudgetAllocation {
    std::vector<std::pair<std::string, float>> categories[6];
};
BirthdayBudgetAllocation birthdayBudgetAlloc;

// Wedding Budget Breakdown
struct WeddingBudgetAllocation {
    std::vector<std::pair<std::string, float>> categories[6];
};
WeddingBudgetAllocation weddingBudgetAlloc;

// Engagement Budget Breakdown
struct EngagementBudgetAllocation {
    std::vector<std::pair<std::string, float>> categories[5];
};
EngagementBudgetAllocation engagementBudgetAlloc;

// Party Budget Breakdown
struct PartyBudgetAllocation {
    std::vector<std::pair<std::string, float>> categories[6];
};
PartyBudgetAllocation partyBudgetAlloc;

void InitializeBudgetAllocations()
{
    // Birthday - Option 1 (Basic, 8,000)
    birthdayBudgetAlloc.categories[0] = {
        {"Venue", 20.0f}, {"Decorations", 8.0f}, {"Catering", 35.0f},
        {"Cake", 7.0f}, {"Entertainment", 5.0f}, {"Activities/Games", 5.0f}, {"Miscellaneous", 20.0f}
    };
    // Birthday - Option 2 (Casual, 12,000)
    birthdayBudgetAlloc.categories[1] = {
        {"Venue", 25.0f}, {"Decorations", 12.0f}, {"Catering", 32.0f},
        {"Cake", 6.0f}, {"Entertainment", 6.0f}, {"Sound System", 5.0f},
        {"Lighting", 3.0f}, {"Photographer", 4.0f}, {"Miscellaneous", 7.0f}
    };
    // Birthday - Option 3 (Balanced, 18,000)
    birthdayBudgetAlloc.categories[2] = {
        {"Venue", 28.0f}, {"Decorations", 14.0f}, {"Catering", 26.0f},
        {"Cake", 7.0f}, {"Entertainment", 8.0f}, {"Sound System", 5.0f},
        {"Lighting", 4.0f}, {"Photographer", 4.0f}, {"Videography", 2.0f},
        {"Party Favors", 1.0f}, {"Activities/Games", 1.0f}
    };
    // Birthday - Option 4 (Aesthetic, 28,000)
    birthdayBudgetAlloc.categories[3] = {
        {"Venue", 32.0f}, {"Decorations", 18.0f}, {"Catering", 22.0f},
        {"Cake", 8.0f}, {"Entertainment", 5.0f}, {"Sound System", 4.0f},
        {"Lighting", 6.0f}, {"Decor Upgrade", 5.0f}, {"Photographer", 5.0f},
        {"Videography", 3.0f}, {"Party Favors", 2.0f}, {"Activities/Games", 2.0f}
    };
    // Birthday - Option 5 (Entertainment-Heavy, 42,000)
    birthdayBudgetAlloc.categories[4] = {
        {"Venue", 25.0f}, {"Decorations", 15.0f}, {"Catering", 20.0f},
        {"Cake", 6.0f}, {"Entertainment", 20.0f}, {"Sound System", 7.0f},
        {"Lighting", 6.0f}, {"Decor Upgrade", 4.0f}, {"Photographer", 4.0f},
        {"Videography", 2.0f}, {"Party Favors", 2.0f}, {"Activities/Games", 8.0f}, {"Miscellaneous", 1.0f}
    };
    // Birthday - Option 6 (Luxury, 60,000)
    birthdayBudgetAlloc.categories[5] = {
        {"Venue", 35.0f}, {"Decorations", 14.0f}, {"Catering", 28.0f},
        {"Cake", 10.0f}, {"Entertainment", 10.0f}, {"Sound System", 6.0f},
        {"Lighting", 7.0f}, {"Decor Upgrade", 5.0f}, {"Photographer", 6.0f},
        {"Videography", 4.0f}, {"Party Favors", 3.0f}, {"Activities/Games", 2.0f}
    };

    // Wedding - Option 1 (Ultra Luxury)
    weddingBudgetAlloc.categories[0] = {
        {"Suit", 7.0f}, {"Wedding Dress", 10.0f}, {"Bouquet", 0.5f},
        {"Makeup/Coiffeur", 1.0f}, {"Venue", 25.0f}, {"Dance Floor", 5.0f},
        {"Lights", 3.0f}, {"Decoration", 3.0f}, {"Photographer", 2.0f},
        {"Catering", 11.5f}, {"Painter", 2.0f}, {"DJ", 5.0f},
        {"Singer", 15.0f}, {"Cinematography", 2.0f}, {"Wedding Favors", 7.0f}, {"Transportation", 1.5f}
    };
    // Wedding - Option 2 (High Luxury)
    weddingBudgetAlloc.categories[1] = {
        {"Suit", 6.0f}, {"Wedding Dress", 12.0f}, {"Bouquet", 1.0f},
        {"Makeup/Coiffeur", 3.0f}, {"Venue", 31.0f}, {"Dance Floor", 4.0f},
        {"Lights", 5.0f}, {"Decoration", 6.0f}, {"Photographer", 6.0f},
        {"Catering", 16.0f}, {"DJ", 5.0f}, {"Cinematography", 2.0f}
    };
    // Wedding - Option 3 (Upper Mid Luxury)
    weddingBudgetAlloc.categories[2] = {
        {"Suit", 6.0f}, {"Wedding Dress", 12.0f}, {"Bouquet", 1.0f},
        {"Makeup/Coiffeur", 3.0f}, {"Venue", 27.0f}, {"Dance Floor", 5.0f},
        {"Lights", 5.0f}, {"Decoration", 6.0f}, {"Photographer", 8.0f},
        {"Catering", 18.0f}, {"DJ", 6.0f}, {"Cinematography", 5.0f}
    };
    // Wedding - Option 4 (Mid Range)
    weddingBudgetAlloc.categories[3] = {
        {"Suit", 5.0f}, {"Wedding Dress", 11.0f}, {"Bouquet", 1.0f},
        {"Makeup/Coiffeur", 3.0f}, {"Venue", 35.0f}, {"Dance Floor", 3.0f},
        {"Lights", 6.0f}, {"Decoration", 10.0f}, {"Photographer", 5.0f},
        {"Catering", 14.0f}, {"DJ", 4.0f}, {"Cinematography", 3.0f}
    };
    // Wedding - Option 5 (Budget-Friendly)
    weddingBudgetAlloc.categories[4] = {
        {"Suit", 5.0f}, {"Wedding Dress", 11.0f}, {"Bouquet", 1.0f},
        {"Makeup/Coiffeur", 3.0f}, {"Venue", 25.0f}, {"Dance Floor", 7.0f},
        {"Lights", 9.0f}, {"Decoration", 7.0f}, {"Photographer", 6.0f},
        {"Catering", 15.0f}, {"Soundsystem", 11.0f}, {"Cinematography", 0.0f}
    };
    // Wedding - Option 6 (Low Budget)
    weddingBudgetAlloc.categories[5] = {
        {"Suit", 7.0f}, {"Wedding Dress", 13.0f}, {"Bouquet", 1.0f},
        {"Dance Floor", 18.0f}, {"Lights", 6.0f}, {"Decoration", 7.0f},
        {"Photographer", 7.0f}, {"Catering", 28.0f}, {"Soundsystem", 15.0f}
    };

    // Engagement - Option 1 (Premium)
    engagementBudgetAlloc.categories[0] = {
        {"Outfit", 12.0f}, {"Makeup/Hair", 5.0f}, {"Venue", 20.0f},
        {"Decor", 18.0f}, {"Lighting", 5.0f}, {"Photographer", 5.0f},
        {"Videography", 5.0f}, {"Catering", 10.0f}, {"DJ/Sound", 7.0f},
        {"Singer/Entertainment", 7.0f}, {"Cake", 3.0f}, {"Favors", 5.0f}, {"Transportation", 3.0f}
    };
    // Engagement - Option 2 (High Tier)
    engagementBudgetAlloc.categories[1] = {
        {"Outfit", 10.0f}, {"Makeup/Hair", 4.0f}, {"Venue", 28.0f},
        {"Decor", 12.0f}, {"Lighting", 7.0f}, {"Photographer", 8.0f},
        {"Videography", 4.0f}, {"Catering", 16.0f}, {"DJ/Sound", 6.0f},
        {"Cake", 2.0f}, {"Favors", 3.0f}
    };
    // Engagement - Option 3 (Upper Mid)
    engagementBudgetAlloc.categories[2] = {
        {"Outfit", 9.0f}, {"Makeup/Hair", 4.0f}, {"Venue", 32.0f},
        {"Decor", 12.0f}, {"Lighting", 8.0f}, {"Photographer", 9.0f},
        {"Catering", 18.0f}, {"DJ/Sound", 6.0f}, {"Cake", 2.0f}
    };
    // Engagement - Option 4 (Mid Range)
    engagementBudgetAlloc.categories[3] = {
        {"Outfit", 8.0f}, {"Makeup/Hair", 3.0f}, {"Venue", 35.0f},
        {"Decor", 10.0f}, {"Lighting", 6.0f}, {"Photographer", 10.0f},
        {"Catering", 22.0f}, {"DJ/Sound", 4.0f}, {"Cake", 2.0f}
    };
    // Engagement - Option 5 (Low Budget)
    engagementBudgetAlloc.categories[4] = {
        {"Outfit", 7.0f}, {"Venue", 20.0f}, {"Decor", 15.0f},
        {"Lighting", 9.0f}, {"Photographer", 12.0f}, {"Catering", 25.0f},
        {"DJ/Sound", 10.0f}, {"Cake", 2.0f}
    };

    // Party - Option 1 (Premium)
    partyBudgetAlloc.categories[0] = {
        {"Venue", 22.0f}, {"Decor", 18.0f}, {"Lighting", 6.0f},
        {"Photographer", 8.0f}, {"Videography", 6.0f}, {"Catering/Food & Drinks", 12.0f},
        {"DJ/Soundsystem", 8.0f}, {"Entertainment", 12.0f}, {"Favors/Giveaways", 5.0f}, {"Transportation", 3.0f}
    };
    // Party - Option 2 (Casual)
    partyBudgetAlloc.categories[1] = {
        {"Venue", 30.0f}, {"Decor", 12.0f}, {"Lighting", 10.0f},
        {"Photographer", 8.0f}, {"Videography", 6.0f}, {"Catering/Food & Drinks", 16.0f},
        {"DJ/Soundsystem", 8.0f}, {"Favors/Giveaways", 10.0f}
    };
    // Party - Option 3 (Balanced)
    partyBudgetAlloc.categories[2] = {
        {"Venue", 28.0f}, {"Decor", 14.0f}, {"Lighting", 4.0f},
        {"Photographer", 5.0f}, {"Videography", 3.0f}, {"Catering/Food & Drinks", 26.0f},
        {"DJ/Soundsystem", 8.0f}, {"Entertainment", 0.0f}, {"Favors/Giveaways", 0.0f}
    };
    // Party - Option 4 (Aesthetic)
    partyBudgetAlloc.categories[3] = {
        {"Venue", 32.0f}, {"Decor", 18.0f}, {"Lighting", 6.0f},
        {"Photographer", 5.0f}, {"Videography", 0.0f}, {"Catering/Food & Drinks", 22.0f},
        {"DJ/Soundsystem", 4.0f}, {"Entertainment", 5.0f}, {"Favors/Giveaways", 2.0f}, {"Transportation", 0.0f}
    };
    // Party - Option 5 (Entertainment-Heavy)
    partyBudgetAlloc.categories[4] = {
        {"Venue", 25.0f}, {"Decor", 15.0f}, {"Lighting", 6.0f},
        {"Photographer", 4.0f}, {"Videography", 2.0f}, {"Catering/Food & Drinks", 20.0f},
        {"DJ/Soundsystem", 7.0f}, {"Entertainment", 20.0f}, {"Favors/Giveaways", 2.0f}
    };
    // Party - Option 6 (Luxury)
    partyBudgetAlloc.categories[5] = {
        {"Venue", 35.0f}, {"Decor", 14.0f}, {"Lighting", 7.0f},
        {"Photographer", 6.0f}, {"Videography", 4.0f}, {"Catering/Food & Drinks", 28.0f},
        {"DJ/Soundsystem", 6.0f}, {"Entertainment", 0.0f}, {"Favors/Giveaways", 3.0f}
    };
}

void RenderEventTypePage()
{
    ImGui::Begin("Create Event - Select Type");

    ImGui::Text("Choose your event type:");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));

    if (ImGui::Button("Wedding", ImVec2(200.0f, 60.0f))) {
        selectedEvent = EventType::Wedding;
        ResetEventOptionState();
        strcpy(selectedEventName, "Wedding");
        selectedEventDate[0] = '\0';
        selectedEventLocation[0] = '\0';
        selectedEventGuests[0] = '\0';
        currentChecklistTasks = nullptr;
        taskDone.clear();
        taskDaysLeft.clear();
    }

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a full wedding event");

    if (ImGui::Button("Engagement", ImVec2(200.0f, 60.0f))) {
        selectedEvent = EventType::Engagement;
        ResetEventOptionState();
        strcpy(selectedEventName, "Engagement");
        selectedEventDate[0] = '\0';
        selectedEventLocation[0] = '\0';
        selectedEventGuests[0] = '\0';
        currentChecklistTasks = nullptr;
        taskDone.clear();
        taskDaysLeft.clear();
    }

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Organize an engagement ceremony");

    if (ImGui::Button("Birthday Party", ImVec2(200.0f, 60.0f))) {
        selectedEvent = EventType::Birthday;
        ResetEventOptionState();
        strcpy(selectedEventName, "Birthday Party");
        selectedEventDate[0] = '\0';
        selectedEventLocation[0] = '\0';
        selectedEventGuests[0] = '\0';
        currentChecklistTasks = nullptr;
        taskDone.clear();
        taskDaysLeft.clear();
    }

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a birthday party");

    if (ImGui::Button("Party", ImVec2(200.0f, 60.0f))) {
        selectedEvent = EventType::Party;
        ResetEventOptionState();
        strcpy(selectedEventName, "Party");
        selectedEventDate[0] = '\0';
        selectedEventLocation[0] = '\0';
        selectedEventGuests[0] = '\0';
        currentChecklistTasks = nullptr;
        taskDone.clear();
        taskDaysLeft.clear();
    }

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a general party");
    if (ImGui::Button("Conference", ImVec2(200.0f, 60.0f))) {
        selectedEvent = EventType::Conference;
        ResetEventOptionState();
        strcpy(selectedEventName, "Conference");
        selectedEventDate[0] = '\0';
        selectedEventLocation[0] = '\0';
        selectedEventGuests[0] = '\0';
        currentChecklistTasks = nullptr;
        taskDone.clear();
        taskDaysLeft.clear();
    }

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a corporate meeting or conference");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Back to Home", ImVec2(160.0f, 35.0f)))
    {
        syncAndSaveCurrentEvent();
        currentScreen = SCREEN_MAIN_MENU;
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

/* void saveEventData(const char *username)
{
    std::string filename = std::string(username) + "_events.txt";
    std::ofstream file(filename);

    if (!file.is_open())
        return;

    // Save event data as key=value pairs
    file << "selectedEvent=" << static_cast<int>(selectedEvent) << std::endl;

    // Birthday data
    file << "birthdayTheme=" << birthdayTheme << std::endl;
    file << "birthdayBudget=" << birthdayBudget << std::endl;
    file << "birthdayGuestList=" << birthdayGuestList << std::endl;
    file << "birthdayDateTime=" << birthdayDateTime << std::endl;
    file << "birthdayInvitations=" << birthdayInvitations << std::endl;
    file << "birthdayVenue=" << birthdayVenue << std::endl;
    file << "birthdayCake=" << birthdayCake << std::endl;
    file << "birthdayDecorations=" << birthdayDecorations << std::endl;
    file << "birthdayFoodDrinks=" << birthdayFoodDrinks << std::endl;
    file << "birthdayPlaylist=" << birthdayPlaylist << std::endl;
    file << "birthdayRSVPs=" << birthdayRSVPs << std::endl;
    file << "birthdaySetupVenue=" << birthdaySetupVenue << std::endl;
    file << "birthdayDecorateSpace=" << birthdayDecorateSpace << std::endl;
    file << "birthdayFoodTable=" << birthdayFoodTable << std::endl;
    file << "birthdayMusic=" << birthdayMusic << std::endl;
    file << "birthdayPhotos=" << birthdayPhotos << std::endl;
    file << "selectedBirthdayOption=" << static_cast<int>(selectedBirthdayOption) << std::endl;

    // Wedding data
    file << "weddingBudget=" << weddingBudget << std::endl;
    file << "weddingDate=" << weddingDate << std::endl;
    file << "weddingVenue=" << weddingVenue << std::endl;
    file << "weddingGuestList=" << weddingGuestList << std::endl;
    file << "weddingPhotographer=" << weddingPhotographer << std::endl;
    file << "weddingVideographer=" << weddingVideographer << std::endl;
    file << "weddingCatering=" << weddingCatering << std::endl;
    file << "weddingInvitations=" << weddingInvitations << std::endl;
    file << "weddingOutfits=" << weddingOutfits << std::endl;
    file << "weddingMakeupArtist=" << weddingMakeupArtist << std::endl;
    file << "weddingSeatingArrangement=" << weddingSeatingArrangement << std::endl;
    file << "weddingFlowers=" << weddingFlowers << std::endl;
    file << "weddingTransport=" << weddingTransport << std::endl;
    file << "weddingVendors=" << weddingVendors << std::endl;
    file << "weddingSchedule=" << weddingSchedule << std::endl;
    file << "weddingSetupVenue=" << weddingSetupVenue << std::endl;
    file << "weddingCeremony=" << weddingCeremony << std::endl;
    file << "weddingTimeline=" << weddingTimeline << std::endl;
    file << "selectedWeddingOption=" << static_cast<int>(selectedWeddingOption) << std::endl;

    // Party data
    file << "partyTheme=" << partyTheme << std::endl;
    file << "partyBudget=" << partyBudget << std::endl;
    file << "partyGuestList=" << partyGuestList << std::endl;
    file << "partyDateTime=" << partyDateTime << std::endl;

    // Budget
    file << "totalBudget=" << totalBudget << std::endl;
    file << "spentBudget=" << spentBudget << std::endl;

    file.close();
}
*/

/* void loadEventData(const char *username)
{
    std::string filename = std::string(username) + "_events.txt";
    std::ifstream file(filename);

    if (!file.is_open())
        return;

    std::string line;
    while (std::getline(file, line))
    {
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos)
            continue;

        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);

        if (key == "selectedEvent")
            selectedEvent = static_cast<EventType>(std::stoi(value));
        else if (key == "birthdayTheme")
            strcpy(birthdayTheme, value.c_str());
        else if (key == "birthdayBudget")
            birthdayBudget = std::stof(value);
        else if (key == "birthdayGuestList")
            strcpy(birthdayGuestList, value.c_str());
        else if (key == "birthdayDateTime")
            strcpy(birthdayDateTime, value.c_str());
        else if (key == "birthdayInvitations")
            strcpy(birthdayInvitations, value.c_str());
        else if (key == "birthdayVenue")
            strcpy(birthdayVenue, value.c_str());
        else if (key == "birthdayCake")
            strcpy(birthdayCake, value.c_str());
        else if (key == "birthdayDecorations")
            strcpy(birthdayDecorations, value.c_str());
        else if (key == "birthdayFoodDrinks")
            strcpy(birthdayFoodDrinks, value.c_str());
        else if (key == "birthdayPlaylist")
            strcpy(birthdayPlaylist, value.c_str());
        else if (key == "birthdayRSVPs")
            strcpy(birthdayRSVPs, value.c_str());
        else if (key == "birthdaySetupVenue")
            strcpy(birthdaySetupVenue, value.c_str());
        else if (key == "birthdayDecorateSpace")
            strcpy(birthdayDecorateSpace, value.c_str());
        else if (key == "birthdayFoodTable")
            strcpy(birthdayFoodTable, value.c_str());
        else if (key == "birthdayMusic")
            strcpy(birthdayMusic, value.c_str());
        else if (key == "birthdayPhotos")
            strcpy(birthdayPhotos, value.c_str());
        else if (key == "selectedBirthdayOption")
            selectedBirthdayOption = static_cast<BirthdayOption>(std::stoi(value));
        else if (key == "weddingBudget")
            weddingBudget = std::stof(value);
        else if (key == "weddingDate")
            strcpy(weddingDate, value.c_str());
        else if (key == "weddingVenue")
            strcpy(weddingVenue, value.c_str());
        else if (key == "weddingGuestList")
            strcpy(weddingGuestList, value.c_str());
        else if (key == "weddingPhotographer")
            strcpy(weddingPhotographer, value.c_str());
        else if (key == "weddingVideographer")
            strcpy(weddingVideographer, value.c_str());
        else if (key == "weddingCatering")
            strcpy(weddingCatering, value.c_str());
        else if (key == "weddingInvitations")
            strcpy(weddingInvitations, value.c_str());
        else if (key == "weddingOutfits")
            strcpy(weddingOutfits, value.c_str());
        else if (key == "weddingMakeupArtist")
            strcpy(weddingMakeupArtist, value.c_str());
        else if (key == "weddingSeatingArrangement")
            strcpy(weddingSeatingArrangement, value.c_str());
        else if (key == "weddingFlowers")
            strcpy(weddingFlowers, value.c_str());
        else if (key == "weddingTransport")
            strcpy(weddingTransport, value.c_str());
        else if (key == "weddingVendors")
            strcpy(weddingVendors, value.c_str());
        else if (key == "weddingSchedule")
            strcpy(weddingSchedule, value.c_str());
        else if (key == "weddingSetupVenue")
            strcpy(weddingSetupVenue, value.c_str());
        else if (key == "weddingCeremony")
            strcpy(weddingCeremony, value.c_str());
        else if (key == "weddingTimeline")
            strcpy(weddingTimeline, value.c_str());
        else if (key == "selectedWeddingOption")
            selectedWeddingOption = static_cast<WeddingOption>(std::stoi(value));
        else if (key == "partyTheme")
            strcpy(partyTheme, value.c_str());
        else if (key == "partyBudget")
            partyBudget = std::stof(value);
        else if (key == "partyGuestList")
            strcpy(partyGuestList, value.c_str());
        else if (key == "partyDateTime")
            strcpy(partyDateTime, value.c_str());
        else if (key == "totalBudget")
            totalBudget = std::stof(value);
        else if (key == "spentBudget")
            spentBudget = std::stof(value);
        else if (key == "taskDone")
        {
            taskDone.clear();
            std::stringstream ss(value);
            std::string item;
            while (std::getline(ss, item, ','))
            {
                taskDone.push_back(item == "1");
            }
        }
        else if (key == "taskDaysLeft")
        {
            taskDaysLeft.clear();
            std::stringstream ss(value);
            std::string item;
            while (std::getline(ss, item, ','))
            {
                taskDaysLeft.push_back(std::stoi(item));
            }
        }
}
*/

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
// Event persistence functions
// ------------------------------------------------------------

// Helper function to escape JSON strings
std::string escapeJsonString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

// Helper function to sanitize filename
std::string sanitizeFilename(const std::string& name) {
    std::string result;
    for (char c : name) {
        if (std::isalnum(c) || c == ' ' || c == '-' || c == '_') {
            result += c;
        } else {
            result += '_'; // Replace invalid chars with underscore
        }
    }
    // Remove leading/trailing spaces and underscores
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](int ch) { return ch != ' ' && ch != '_'; }));
    result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch) { return ch != ' ' && ch != '_'; }).base(), result.end());
    if (result.empty()) result = "unnamed_event";
    return result;
}

void saveEvent(const Event& event) {
    std::string safeName = sanitizeFilename(event.name);
    std::string filename = "events/" + safeName + ".json";
    std::filesystem::create_directories("events");
    std::ofstream file(filename);
    if (!file.is_open()) return;

    file << "{\n";
    file << "  \"name\": \"" << escapeJsonString(event.name) << "\",\n";
    file << "  \"date\": \"" << escapeJsonString(event.date) << "\",\n";
    file << "  \"location\": \"" << escapeJsonString(event.location) << "\",\n";
    file << "  \"budget\": " << event.budget << ",\n";
    file << "  \"guestList\": \"" << escapeJsonString(event.guestList) << "\",\n";
    file << "  \"notes\": \"" << escapeJsonString(event.notes) << "\",\n";
    file << "  \"type\": " << static_cast<int>(event.type) << ",\n";
    file << "  \"selectedOption\": " << event.selectedOption << ",\n";
    file << "  \"tasks\": [\n";
    for (size_t i = 0; i < event.tasks.size(); ++i) {
        file << "    \"" << escapeJsonString(event.tasks[i]) << "\"";
        if (i < event.tasks.size() - 1) file << ",";
        file << "\n";
    }
    file << "  ],\n";
    file << "  \"taskCompleted\": [\n";
    for (size_t i = 0; i < event.taskCompleted.size(); ++i) {
        file << "    " << (event.taskCompleted[i] ? "true" : "false");
        if (i < event.taskCompleted.size() - 1) file << ",";
        file << "\n";
    }
    file << "  ]\n";
    file << "}\n";
    
    file.close();
}

// Helper function to unescape JSON strings
std::string unescapeJsonString(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case '"': result += '"'; i++; break;
                case '\\': result += '\\'; i++; break;
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

void loadEvents() {
    savedEvents.clear();
    std::filesystem::create_directories("events");
    
    for (const auto& entry : std::filesystem::directory_iterator("events")) {
        if (entry.path().extension() == ".json") {
            std::ifstream file(entry.path());
            if (!file.is_open()) continue;
            
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            
            Event event;
            
            // Simple JSON parsing using string positions
            auto findJsonValue = [&](const std::string& key) -> std::string {
                std::string search = "\"" + key + "\":";
                size_t pos = content.find(search);
                if (pos == std::string::npos) return "";
                
                pos += search.length();
                // Skip whitespace
                while (pos < content.length() && std::isspace(content[pos])) pos++;
                
                if (content[pos] == '"') {
                    // String value
                    pos++;
                    size_t start = pos;
                    while (pos < content.length() && content[pos] != '"') {
                        if (content[pos] == '\\') pos++; // Skip escaped chars
                        pos++;
                    }
                    return content.substr(start, pos - start);
                } else {
                    // Number or boolean
                    size_t start = pos;
                    while (pos < content.length() && content[pos] != ',' && content[pos] != '}' && content[pos] != ']') {
                        pos++;
                    }
                    std::string val = content.substr(start, pos - start);
                    // Trim whitespace
                    val.erase(val.begin(), std::find_if(val.begin(), val.end(), [](int ch) { return !std::isspace(ch); }));
                    val.erase(std::find_if(val.rbegin(), val.rend(), [](int ch) { return !std::isspace(ch); }).base(), val.end());
                    return val;
                }
            };
            
            event.name = unescapeJsonString(findJsonValue("name"));
            event.date = unescapeJsonString(findJsonValue("date"));
            event.location = unescapeJsonString(findJsonValue("location"));
            event.guestList = unescapeJsonString(findJsonValue("guestList"));
            event.notes = unescapeJsonString(findJsonValue("notes"));
            
            std::string budgetStr = findJsonValue("budget");
            if (!budgetStr.empty()) {
                try { event.budget = std::stof(budgetStr); } catch (...) { event.budget = 0.0f; }
            }
            
            std::string typeStr = findJsonValue("type");
            if (!typeStr.empty()) {
                try { event.type = static_cast<EventType>(std::stoi(typeStr)); } catch (...) { event.type = EventType::None; }
            }
            
            std::string optionStr = findJsonValue("selectedOption");
            if (!optionStr.empty()) {
                try { event.selectedOption = std::stoi(optionStr); } catch (...) { event.selectedOption = 0; }
            } else {
                event.selectedOption = 0;
            }
            
            // Parse tasks array
            std::string tasksKey = "\"tasks\":";
            size_t tasksPos = content.find(tasksKey);
            if (tasksPos != std::string::npos) {
                tasksPos += tasksKey.length();
                size_t arrayStart = content.find('[', tasksPos);
                size_t arrayEnd = content.find(']', arrayStart);
                if (arrayStart != std::string::npos && arrayEnd != std::string::npos) {
                    std::string arrayContent = content.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                    std::stringstream ss(arrayContent);
                    std::string item;
                    while (std::getline(ss, item, ',')) {
                        // Trim whitespace and quotes
                        item.erase(item.begin(), std::find_if(item.begin(), item.end(), [](int ch) { return !std::isspace(ch) && ch != '"'; }));
                        item.erase(std::find_if(item.rbegin(), item.rend(), [](int ch) { return !std::isspace(ch) && ch != '"'; }).base(), item.end());
                        if (!item.empty()) {
                            event.tasks.push_back(unescapeJsonString(item));
                        }
                    }
                }
            }
            
            // Parse taskCompleted array
            std::string completedKey = "\"taskCompleted\":";
            size_t completedPos = content.find(completedKey);
            if (completedPos != std::string::npos) {
                completedPos += completedKey.length();
                size_t arrayStart = content.find('[', completedPos);
                size_t arrayEnd = content.find(']', arrayStart);
                if (arrayStart != std::string::npos && arrayEnd != std::string::npos) {
                    std::string arrayContent = content.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                    std::stringstream ss(arrayContent);
                    std::string item;
                    while (std::getline(ss, item, ',')) {
                        // Trim whitespace
                        item.erase(item.begin(), std::find_if(item.begin(), item.end(), [](int ch) { return !std::isspace(ch); }));
                        item.erase(std::find_if(item.rbegin(), item.rend(), [](int ch) { return !std::isspace(ch); }).base(), item.end());
                        event.taskCompleted.push_back(item == "true");
                    }
                }
            }
            
            // Ensure taskCompleted has same size as tasks
            while (event.taskCompleted.size() < event.tasks.size()) {
                event.taskCompleted.push_back(false);
            }
            
            savedEvents.push_back(event);
        }
    }
}

// ------------------------------------------------------------
// Create event with option and budget
// ------------------------------------------------------------

void CreateEventWithOption(int optionIndex, float budget, const std::string& defaultName) {
    Event newEvent;
    newEvent.name = defaultName;
    newEvent.date = "2026-05-11"; // default date
    newEvent.location = "";
    newEvent.budget = budget;
    newEvent.guestList = "";
    newEvent.notes = "";
    newEvent.type = selectedEvent;
    newEvent.selectedOption = optionIndex;
    
    // Set tasks based on type and option
    if (selectedEvent == EventType::Birthday) {
        if (optionIndex == 1) {
            newEvent.tasks.assign(birthdayOption1Tasks.begin(), birthdayOption1Tasks.end());
        } else if (optionIndex == 2) {
            newEvent.tasks.assign(birthdayOption2Tasks.begin(), birthdayOption2Tasks.end());
        } else if (optionIndex == 3) {
            newEvent.tasks.assign(birthdayOption3Tasks.begin(), birthdayOption3Tasks.end());
        } else if (optionIndex == 4) {
            newEvent.tasks.assign(birthdayOption4Tasks.begin(), birthdayOption4Tasks.end());
        } else if (optionIndex == 5) {
            newEvent.tasks.assign(birthdayOption5Tasks.begin(), birthdayOption5Tasks.end());
        } else if (optionIndex == 6) {
            newEvent.tasks.assign(birthdayOption6Tasks.begin(), birthdayOption6Tasks.end());
        }
    } else if (selectedEvent == EventType::Wedding) {
        if (optionIndex == 1) {
            newEvent.tasks.assign(weddingOption1Tasks.begin(), weddingOption1Tasks.end());
        } else if (optionIndex == 2) {
            newEvent.tasks.assign(weddingOption2Tasks.begin(), weddingOption2Tasks.end());
        } else if (optionIndex == 3) {
            newEvent.tasks.assign(weddingOption3Tasks.begin(), weddingOption3Tasks.end());
        } else if (optionIndex == 4) {
            newEvent.tasks.assign(weddingOption4Tasks.begin(), weddingOption4Tasks.end());
        } else if (optionIndex == 5) {
            newEvent.tasks.assign(weddingOption5Tasks.begin(), weddingOption5Tasks.end());
        } else if (optionIndex == 6) {
            newEvent.tasks.assign(weddingOption6Tasks.begin(), weddingOption6Tasks.end());
        }
    } else if (selectedEvent == EventType::Party) {
        if (optionIndex == 1) {
            newEvent.tasks.assign(partyOption1Tasks.begin(), partyOption1Tasks.end());
        } else if (optionIndex == 2) {
            newEvent.tasks.assign(partyOption2Tasks.begin(), partyOption2Tasks.end());
        } else if (optionIndex == 3) {
            newEvent.tasks.assign(partyOption3Tasks.begin(), partyOption3Tasks.end());
        } else if (optionIndex == 4) {
            newEvent.tasks.assign(partyOption4Tasks.begin(), partyOption4Tasks.end());
        } else if (optionIndex == 5) {
            newEvent.tasks.assign(partyOption5Tasks.begin(), partyOption5Tasks.end());
        }
    } else if (selectedEvent == EventType::Conference) {
        if (optionIndex == 1) {
            newEvent.tasks.assign(conferenceOption1Tasks.begin(), conferenceOption1Tasks.end());
        } else if (optionIndex == 2) {
            newEvent.tasks.assign(conferenceOption2Tasks.begin(), conferenceOption2Tasks.end());
        } else if (optionIndex == 3) {
            newEvent.tasks.assign(conferenceOption3Tasks.begin(), conferenceOption3Tasks.end());
        } else if (optionIndex == 4) {
            newEvent.tasks.assign(conferenceOption4Tasks.begin(), conferenceOption4Tasks.end());
        } else if (optionIndex == 5) {
            newEvent.tasks.assign(conferenceOption5Tasks.begin(), conferenceOption5Tasks.end());
        } else if (optionIndex == 6) {
            newEvent.tasks.assign(conferenceOption6Tasks.begin(), conferenceOption6Tasks.end());
        }
    } else if (selectedEvent == EventType::Engagement) {
        if (optionIndex == 1) {
            newEvent.tasks.assign(engagementOption1Tasks.begin(), engagementOption1Tasks.end());
        } else if (optionIndex == 2) {
            newEvent.tasks.assign(engagementOption2Tasks.begin(), engagementOption2Tasks.end());
        } else if (optionIndex == 3) {
            newEvent.tasks.assign(engagementOption3Tasks.begin(), engagementOption3Tasks.end());
        } else if (optionIndex == 4) {
            newEvent.tasks.assign(engagementOption4Tasks.begin(), engagementOption4Tasks.end());
        } else if (optionIndex == 5) {
            newEvent.tasks.assign(engagementOption5Tasks.begin(), engagementOption5Tasks.end());
        }
    }
    
    newEvent.taskCompleted.assign(newEvent.tasks.size(), false);
    
    savedEvents.push_back(newEvent);
    currentEvent = &savedEvents.back();
    saveEvent(newEvent);
    
    // Set dashboard variables
    strcpy(selectedEventName, newEvent.name.c_str());
    strcpy(selectedEventDate, newEvent.date.c_str());
    strcpy(selectedEventLocation, newEvent.location.c_str());
    strcpy(selectedEventGuests, newEvent.guestList.c_str());
    totalBudget = newEvent.budget;
    spentBudget = 0.0f;
    
    // Set type-specific budget and option
    if (newEvent.type == EventType::Birthday) {
        birthdayBudget = newEvent.budget;
        selectedBirthdayOption = static_cast<BirthdayOption>(optionIndex);
    } else if (newEvent.type == EventType::Wedding) {
        weddingBudget = newEvent.budget;
        selectedWeddingOption = static_cast<WeddingOption>(optionIndex);
    } else if (newEvent.type == EventType::Party) {
        partyBudget = newEvent.budget;
        selectedPartyOption = static_cast<PartyOption>(optionIndex);
    } else if (newEvent.type == EventType::Conference) {
        conferenceBudget = newEvent.budget;
        selectedConferenceOption = static_cast<ConferenceOption>(optionIndex);
    } else if (newEvent.type == EventType::Engagement) {
        engagementBudget = newEvent.budget;
        selectedEngagementOption = static_cast<EngagementOption>(optionIndex);
    }
    
    // Set up tasks
    currentEventTasks = newEvent.tasks;
    UpdateDynamicTasks(currentEventTasks);
    taskDone = newEvent.taskCompleted;
    
    currentScreen = SCREEN_EVENT_DETAILS;
}

// ------------------------------------------------------------
// Sync and save current event
// ------------------------------------------------------------

void syncAndSaveCurrentEvent() {
    if (!currentEvent) return;
    
    // Sync from dashboard variables
    currentEvent->name = selectedEventName;
    currentEvent->date = selectedEventDate;
    currentEvent->location = selectedEventLocation;
    currentEvent->guestList = selectedEventGuests;
    currentEvent->budget = totalBudget;
    
    // Sync tasks
    if (currentDynamicTasks) {
        currentEvent->tasks = *currentDynamicTasks;
    } else if (!currentEventTasks.empty()) {
        currentEvent->tasks = currentEventTasks;
    }
    currentEvent->taskCompleted = taskDone;
    
    // Sync type-specific budgets
    if (currentEvent->type == EventType::Birthday) {
        currentEvent->budget = birthdayBudget;
        strcpy(birthdayTheme, currentEvent->name.c_str());
        strcpy(birthdayDateTime, currentEvent->date.c_str());
        strcpy(birthdayVenue, currentEvent->location.c_str());
        strcpy(birthdayGuestList, currentEvent->guestList.c_str());
    } else if (currentEvent->type == EventType::Wedding) {
        currentEvent->budget = weddingBudget;
        strcpy(weddingDate, currentEvent->date.c_str());
        strcpy(weddingVenue, currentEvent->location.c_str());
        strcpy(weddingGuestList, currentEvent->guestList.c_str());
    } else if (currentEvent->type == EventType::Party) {
        currentEvent->budget = partyBudget;
        strcpy(partyTheme, currentEvent->name.c_str());
        strcpy(partyDateTime, currentEvent->date.c_str());
        strcpy(partyGuestList, currentEvent->guestList.c_str());
    } else if (currentEvent->type == EventType::Engagement) {
        currentEvent->budget = engagementBudget;
    } else if (currentEvent->type == EventType::Conference) {
        currentEvent->budget = conferenceBudget;
    }
    
    // Update global budget
    totalBudget = currentEvent->budget;
    
    saveEvent(*currentEvent);
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
    SetOptionBudgetRange();

    // ================= PARTY =================
    if (selectedEvent == EventType::Party)
    {
        ImGui::Text("Party Event Details");
        ImGui::Separator();

        if (selectedPartyOption == PartyOption::None)
        {
            ImGui::Text("Choose Party Option:");
            ImGui::Spacing();

            if (ImGui::Button("Basic")) {
                selectedPartyOption = PartyOption::Option1;
                selectedOptionName = "Basic";
                partyMinBudget = 6000.0f;
                partyMaxBudget = 10000.0f;
                budgetRangeText = "Recommended budget: 6,000 - 10,000 EGP";
                UpdateChecklistTasks(partyOption1Tasks);
            }
            if (ImGui::Button("Mid")) {
                selectedPartyOption = PartyOption::Option2;
                selectedOptionName = "Mid";
                partyMinBudget = 10000.0f;
                partyMaxBudget = 14000.0f;
                budgetRangeText = "Recommended budget: 10,000 - 14,000 EGP";
                UpdateChecklistTasks(partyOption2Tasks);
            }
            if (ImGui::Button("Premium")) {
                selectedPartyOption = PartyOption::Option3;
                selectedOptionName = "Premium";
                partyMinBudget = 15000.0f;
                partyMaxBudget = 20000.0f;
                budgetRangeText = "Recommended budget: 15,000 - 20,000 EGP";
                UpdateChecklistTasks(partyOption3Tasks);
            }
            if (ImGui::Button("Deluxe")) {
                selectedPartyOption = PartyOption::Option4;
                selectedOptionName = "Deluxe";
                partyMinBudget = 24000.0f;
                partyMaxBudget = 32000.0f;
                budgetRangeText = "Recommended budget: 24,000 - 32,000 EGP";
                UpdateChecklistTasks(partyOption4Tasks);
            }
            if (ImGui::Button("Luxury")) {
                selectedPartyOption = PartyOption::Option5;
                selectedOptionName = "Luxury";
                partyMinBudget = 38000.0f;
                partyMaxBudget = 46000.0f;
                budgetRangeText = "Recommended budget: 38,000 - 46,000 EGP";
                UpdateChecklistTasks(partyOption5Tasks);
            }
            if (ImGui::Button("Ultra Luxury")) {
                selectedPartyOption = PartyOption::Option6;
                selectedOptionName = "Ultra Luxury";
                partyMinBudget = 54000.0f;
                partyMaxBudget = 66000.0f;
                budgetRangeText = "Recommended budget: 54,000 - 66,000 EGP";
                UpdateChecklistTasks(partyOption6Tasks);
            }
        }
        else
        {
            ImGui::InputFloat("Budget (EGP)", &partyBudget, 100.0f, 1000.0f, "%.0f");
            ImGui::Text("%s", budgetRangeText.c_str());
            ImGui::Spacing();

            bool budgetValid = (partyBudget >= partyMinBudget && partyBudget <= partyMaxBudget);
            if (!budgetValid && partyBudget > 0) {
                ImGui::TextColored(ImVec4(1,0,0,1), "Budget must be between %.0f and %.0f EGP", partyMinBudget, partyMaxBudget);
            }
            ImGui::Spacing();

            ImGui::InputText("Date", selectedEventDate, IM_ARRAYSIZE(selectedEventDate));
            ImGui::InputText("Location/Venue", selectedEventLocation, IM_ARRAYSIZE(selectedEventLocation));
            ImGui::InputText("Number of Guests", selectedEventGuests, IM_ARRAYSIZE(selectedEventGuests));
            ImGui::Spacing();
            ImGui::Text("Remaining tasks for this option will appear in the dashboard checklist.");
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

            if (ImGui::Button("Ultra Luxury")) {
                selectedWeddingOption = WeddingOption::Option1;
                selectedOptionName = "Ultra Luxury";
                weddingMinBudget = 9000000.0f;
                weddingMaxBudget = 11000000.0f;
                budgetRangeText = "Recommended budget: 9,000,000 - 11,000,000 EGP";
                UpdateChecklistTasks(weddingOption1Tasks);
            }
            if (ImGui::Button("Luxury")) {
                selectedWeddingOption = WeddingOption::Option2;
                selectedOptionName = "Luxury";
                weddingMinBudget = 5000000.0f;
                weddingMaxBudget = 6000000.0f;
                budgetRangeText = "Recommended budget: 5,000,000 - 6,000,000 EGP";
                UpdateChecklistTasks(weddingOption2Tasks);
            }
            if (ImGui::Button("Premium")) {
                selectedWeddingOption = WeddingOption::Option3;
                selectedOptionName = "Premium";
                weddingMinBudget = 2700000.0f;
                weddingMaxBudget = 3300000.0f;
                budgetRangeText = "Recommended budget: 2,700,000 - 3,300,000 EGP";
                UpdateChecklistTasks(weddingOption3Tasks);
            }
            if (ImGui::Button("Mid")) {
                selectedWeddingOption = WeddingOption::Option4;
                selectedOptionName = "Mid";
                weddingMinBudget = 1350000.0f;
                weddingMaxBudget = 1650000.0f;
                budgetRangeText = "Recommended budget: 1,350,000 - 1,650,000 EGP";
                UpdateChecklistTasks(weddingOption4Tasks);
            }
            if (ImGui::Button("Budget")) {
                selectedWeddingOption = WeddingOption::Option5;
                selectedOptionName = "Budget";
                weddingMinBudget = 720000.0f;
                weddingMaxBudget = 880000.0f;
                budgetRangeText = "Recommended budget: 720,000 - 880,000 EGP";
                UpdateChecklistTasks(weddingOption5Tasks);
            }
            if (ImGui::Button("Basic")) {
                selectedWeddingOption = WeddingOption::Option6;
                selectedOptionName = "Basic";
                weddingMinBudget = 135000.0f;
                weddingMaxBudget = 165000.0f;
                budgetRangeText = "Recommended budget: 135,000 - 165,000 EGP";
                UpdateChecklistTasks(weddingOption6Tasks);
            }
        }
        else
        {
            ImGui::InputFloat("Budget (EGP)", &weddingBudget, 1000.0f, 10000.0f, "%.0f");
            ImGui::Text("%s", budgetRangeText.c_str());
            ImGui::Spacing();

            bool budgetValid = (weddingBudget >= weddingMinBudget && weddingBudget <= weddingMaxBudget);
            if (!budgetValid && weddingBudget > 0) {
                ImGui::TextColored(ImVec4(1,0,0,1), "Budget must be between %.0f and %.0f EGP", weddingMinBudget, weddingMaxBudget);
            }
            ImGui::Spacing();

            ImGui::InputText("Date", selectedEventDate, IM_ARRAYSIZE(selectedEventDate));
            ImGui::InputText("Location/Venue", selectedEventLocation, IM_ARRAYSIZE(selectedEventLocation));
            ImGui::InputText("Number of Guests", selectedEventGuests, IM_ARRAYSIZE(selectedEventGuests));
            ImGui::Spacing();
            ImGui::Text("Remaining tasks for this option will appear in the dashboard checklist.");

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

            if (ImGui::Button("Basic")) {
                selectedBirthdayOption = BirthdayOption::Option1;
                selectedOptionName = "Basic";
                birthdayMinBudget = 7000.0f;
                birthdayMaxBudget = 9000.0f;
                budgetRangeText = "Recommended budget: 7,000 - 9,000 EGP";
                UpdateChecklistTasks(birthdayOption1Tasks);
            }
            if (ImGui::Button("Mid")) {
                selectedBirthdayOption = BirthdayOption::Option2;
                selectedOptionName = "Mid";
                birthdayMinBudget = 11000.0f;
                birthdayMaxBudget = 13000.0f;
                budgetRangeText = "Recommended budget: 11,000 - 13,000 EGP";
                UpdateChecklistTasks(birthdayOption2Tasks);
            }
            if (ImGui::Button("Premium")) {
                selectedBirthdayOption = BirthdayOption::Option3;
                selectedOptionName = "Premium";
                birthdayMinBudget = 16000.0f;
                birthdayMaxBudget = 20000.0f;
                budgetRangeText = "Recommended budget: 16,000 - 20,000 EGP";
                UpdateChecklistTasks(birthdayOption3Tasks);
            }
            if (ImGui::Button("Deluxe")) {
                selectedBirthdayOption = BirthdayOption::Option4;
                selectedOptionName = "Deluxe";
                birthdayMinBudget = 25000.0f;
                birthdayMaxBudget = 31000.0f;
                budgetRangeText = "Recommended budget: 25,000 - 31,000 EGP";
                UpdateChecklistTasks(birthdayOption4Tasks);
            }
            if (ImGui::Button("Luxury")) {
                selectedBirthdayOption = BirthdayOption::Option5;
                selectedOptionName = "Luxury";
                birthdayMinBudget = 39000.0f;
                birthdayMaxBudget = 45000.0f;
                budgetRangeText = "Recommended budget: 39,000 - 45,000 EGP";
                UpdateChecklistTasks(birthdayOption5Tasks);
            }
            if (ImGui::Button("Ultra Luxury")) {
                selectedBirthdayOption = BirthdayOption::Option6;
                selectedOptionName = "Ultra Luxury";
                birthdayMinBudget = 54000.0f;
                birthdayMaxBudget = 66000.0f;
                budgetRangeText = "Recommended budget: 54,000 - 66,000 EGP";
                UpdateChecklistTasks(birthdayOption6Tasks);
            }
        }
        else
        {
            ImGui::InputFloat("Budget (EGP)", &birthdayBudget, 100.0f, 1000.0f, "%.0f");
            ImGui::Text("%s", budgetRangeText.c_str());
            ImGui::Spacing();

            bool budgetValid = (birthdayBudget >= birthdayMinBudget && birthdayBudget <= birthdayMaxBudget);
            if (!budgetValid && birthdayBudget > 0) {
                ImGui::TextColored(ImVec4(1,0,0,1), "Budget must be between %.0f and %.0f EGP", birthdayMinBudget, birthdayMaxBudget);
            }
            ImGui::Spacing();

            ImGui::InputText("Date", selectedEventDate, IM_ARRAYSIZE(selectedEventDate));
            ImGui::InputText("Location/Venue", selectedEventLocation, IM_ARRAYSIZE(selectedEventLocation));
            ImGui::InputText("Number of Guests", selectedEventGuests, IM_ARRAYSIZE(selectedEventGuests));
            ImGui::Spacing();
            ImGui::Text("Remaining tasks for this option will appear in the dashboard checklist.");

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

            if (ImGui::Button("Premium")) {
                selectedEngagementOption = EngagementOption::Option1;
                selectedOptionName = "Premium";
                engagementMinBudget = 1800000.0f;
                engagementMaxBudget = 2200000.0f;
                budgetRangeText = "Recommended budget: 1,800,000 - 2,200,000 EGP";
                UpdateChecklistTasks(engagementOption1Tasks);
            }
            if (ImGui::Button("Luxury")) {
                selectedEngagementOption = EngagementOption::Option2;
                selectedOptionName = "Luxury";
                engagementMinBudget = 900000.0f;
                engagementMaxBudget = 1100000.0f;
                budgetRangeText = "Recommended budget: 900,000 - 1,100,000 EGP";
                UpdateChecklistTasks(engagementOption2Tasks);
            }
            if (ImGui::Button("Mid")) {
                selectedEngagementOption = EngagementOption::Option3;
                selectedOptionName = "Mid";
                engagementMinBudget = 540000.0f;
                engagementMaxBudget = 660000.0f;
                budgetRangeText = "Recommended budget: 540,000 - 660,000 EGP";
                UpdateChecklistTasks(engagementOption3Tasks);
            }
            if (ImGui::Button("Basic")) {
                selectedEngagementOption = EngagementOption::Option4;
                selectedOptionName = "Basic";
                engagementMinBudget = 270000.0f;
                engagementMaxBudget = 330000.0f;
                budgetRangeText = "Recommended budget: 270,000 - 330,000 EGP";
                UpdateChecklistTasks(engagementOption4Tasks);
            }
            if (ImGui::Button("Budget")) {
                selectedEngagementOption = EngagementOption::Option5;
                selectedOptionName = "Budget";
                engagementMinBudget = 72000.0f;
                engagementMaxBudget = 88000.0f;
                budgetRangeText = "Recommended budget: 72,000 - 88,000 EGP";
                UpdateChecklistTasks(engagementOption5Tasks);
            }
        }
        else
        {
            ImGui::InputFloat("Budget (EGP)", &engagementBudget, 1000.0f, 10000.0f, "%.0f");
            ImGui::Text("%s", budgetRangeText.c_str());
            ImGui::Spacing();

            bool budgetValid = (engagementBudget >= engagementMinBudget && engagementBudget <= engagementMaxBudget);
            if (!budgetValid && engagementBudget > 0) {
                ImGui::TextColored(ImVec4(1,0,0,1), "Budget must be between %.0f and %.0f EGP", engagementMinBudget, engagementMaxBudget);
            }
            ImGui::Spacing();

            ImGui::InputText("Date", selectedEventDate, IM_ARRAYSIZE(selectedEventDate));
            ImGui::InputText("Location/Venue", selectedEventLocation, IM_ARRAYSIZE(selectedEventLocation));
            ImGui::InputText("Number of Guests", selectedEventGuests, IM_ARRAYSIZE(selectedEventGuests));
            ImGui::Spacing();
            ImGui::Text("Remaining tasks for this option will appear in the dashboard checklist.");

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

            if (ImGui::Button("Basic")) {
                selectedConferenceOption = ConferenceOption::Option1;
                selectedOptionName = "Basic";
                conferenceMinBudget = 5000.0f;
                conferenceMaxBudget = 10000.0f;
                budgetRangeText = "Recommended budget: 5,000 - 10,000 EGP";
                UpdateChecklistTasks(conferenceOption1Tasks);
            }
            if (ImGui::Button("Mid")) {
                selectedConferenceOption = ConferenceOption::Option2;
                selectedOptionName = "Mid";
                conferenceMinBudget = 10000.0f;
                conferenceMaxBudget = 20000.0f;
                budgetRangeText = "Recommended budget: 10,000 - 20,000 EGP";
                UpdateChecklistTasks(conferenceOption2Tasks);
            }
            if (ImGui::Button("Premium")) {
                selectedConferenceOption = ConferenceOption::Option3;
                selectedOptionName = "Premium";
                conferenceMinBudget = 20000.0f;
                conferenceMaxBudget = 40000.0f;
                budgetRangeText = "Recommended budget: 20,000 - 40,000 EGP";
                UpdateChecklistTasks(conferenceOption3Tasks);
            }
            if (ImGui::Button("Deluxe")) {
                selectedConferenceOption = ConferenceOption::Option4;
                selectedOptionName = "Deluxe";
                conferenceMinBudget = 40000.0f;
                conferenceMaxBudget = 70000.0f;
                budgetRangeText = "Recommended budget: 40,000 - 70,000 EGP";
                UpdateChecklistTasks(conferenceOption4Tasks);
            }
            if (ImGui::Button("Luxury")) {
                selectedConferenceOption = ConferenceOption::Option5;
                selectedOptionName = "Luxury";
                conferenceMinBudget = 70000.0f;
                conferenceMaxBudget = 100000.0f;
                budgetRangeText = "Recommended budget: 70,000 - 100,000 EGP";
                UpdateChecklistTasks(conferenceOption5Tasks);
            }
            if (ImGui::Button("Elite")) {
                selectedConferenceOption = ConferenceOption::Option6;
                selectedOptionName = "Elite";
                conferenceMinBudget = 100000.0f;
                conferenceMaxBudget = 200000.0f;
                budgetRangeText = "Recommended budget: 100,000 - 200,000 EGP";
                UpdateChecklistTasks(conferenceOption6Tasks);
            }
        }
        else
        {
            ImGui::InputFloat("Budget (EGP)", &conferenceBudget, 1000.0f, 10000.0f, "%.0f");
            ImGui::Text("%s", budgetRangeText.c_str());
            ImGui::Spacing();

            bool budgetValid = (conferenceBudget >= conferenceMinBudget && conferenceBudget <= conferenceMaxBudget);
            if (!budgetValid && conferenceBudget > 0) {
                ImGui::TextColored(ImVec4(1,0,0,1), "Budget must be between %.0f and %.0f EGP", conferenceMinBudget, conferenceMaxBudget);
            }
            ImGui::Spacing();

            ImGui::InputText("Date", selectedEventDate, IM_ARRAYSIZE(selectedEventDate));
            ImGui::InputText("Location/Venue", selectedEventLocation, IM_ARRAYSIZE(selectedEventLocation));
            ImGui::InputText("Number of Guests", selectedEventGuests, IM_ARRAYSIZE(selectedEventGuests));
            ImGui::Spacing();
            ImGui::Text("Remaining tasks for this option will appear in the dashboard checklist.");

            ImGui::Spacing();
            if (ImGui::Button("Change Option"))
            {
                selectedConferenceOption = ConferenceOption::None; // reset so user can re-choose
            }
        }
    }

    ImGui::Spacing();

    bool canProceed = false;
    if (selectedEvent == EventType::Wedding) {
        canProceed = (weddingBudget >= weddingMinBudget && weddingBudget <= weddingMaxBudget && weddingBudget > 0);
    } else if (selectedEvent == EventType::Birthday) {
        canProceed = (birthdayBudget >= birthdayMinBudget && birthdayBudget <= birthdayMaxBudget && birthdayBudget > 0);
    } else if (selectedEvent == EventType::Engagement) {
        canProceed = (engagementBudget >= engagementMinBudget && engagementBudget <= engagementMaxBudget && engagementBudget > 0);
    } else if (selectedEvent == EventType::Party) {
        canProceed = (partyBudget >= partyMinBudget && partyBudget <= partyMaxBudget && partyBudget > 0);
    } else if (selectedEvent == EventType::Conference) {
        canProceed = (conferenceBudget >= conferenceMinBudget && conferenceBudget <= conferenceMaxBudget && conferenceBudget > 0);
    }

    if (!canProceed) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Go to Dashboard"))
    {
        currentScreen = SCREEN_DASHBOARD;
    }
    if (!canProceed) {
        ImGui::EndDisabled();
    }

    if (ImGui::Button("Back"))
    {
        currentScreen = SCREEN_EVENT_OPTIONS;
    }

    ImGui::End();
}

void RenderBudgetSystem()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Budget System", NULL, 
                 ImGuiWindowFlags_NoMove | 
                 ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoCollapse | 
                 ImGuiWindowFlags_NoTitleBar);

    ImGui::Text("Budget Allocation System");
    ImGui::Separator();
    ImGui::Spacing();

    float totalBudgetForEvent = 0.0f;
    std::vector<std::pair<std::string, float>> *currentAllocation = nullptr;

    // Determine which budget and allocation to use
    if (selectedEvent == EventType::Birthday) {
        totalBudgetForEvent = birthdayBudget;
        if (selectedBirthdayOption != BirthdayOption::None) {
            currentAllocation = &birthdayBudgetAlloc.categories[(int)selectedBirthdayOption - 1];
        }
    } else if (selectedEvent == EventType::Wedding) {
        totalBudgetForEvent = weddingBudget;
        if (selectedWeddingOption != WeddingOption::None) {
            currentAllocation = &weddingBudgetAlloc.categories[(int)selectedWeddingOption - 1];
        }
    } else if (selectedEvent == EventType::Engagement) {
        totalBudgetForEvent = engagementBudget;
        if (selectedEngagementOption != EngagementOption::None) {
            currentAllocation = &engagementBudgetAlloc.categories[(int)selectedEngagementOption - 1];
        }
    } else if (selectedEvent == EventType::Party) {
        totalBudgetForEvent = partyBudget;
        if (selectedPartyOption != PartyOption::None) {
            currentAllocation = &partyBudgetAlloc.categories[(int)selectedPartyOption - 1];
        }
    }

    if (totalBudgetForEvent <= 0.0f) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a budget amount first!");
        ImGui::Spacing();
    } else if (currentAllocation == nullptr || currentAllocation->empty()) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "No allocation data available for this option.");
        ImGui::Spacing();
    } else {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Total Budget: %.0f EGP", totalBudgetForEvent);
        ImGui::Separator();
        ImGui::Spacing();

        // Display table header
        ImGui::Columns(3, "budget_breakdown", true);
        ImGui::SetColumnWidth(0, 250);
        ImGui::SetColumnWidth(1, 100);
        ImGui::SetColumnWidth(2, 150);

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Category");
        ImGui::NextColumn();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Percentage");
        ImGui::NextColumn();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Suggested Amount");
        ImGui::NextColumn();
        ImGui::Separator();

        // Display each category with calculated amount
        for (const auto &item : *currentAllocation) {
            float suggestedAmount = (item.second / 100.0f) * totalBudgetForEvent;
            
            ImGui::Text("%s", item.first.c_str());
            ImGui::NextColumn();
            ImGui::Text("%.1f%%", item.second);
            ImGui::NextColumn();
            ImGui::TextColored(ImVec4(0, 1, 1, 1), "%.0f EGP", suggestedAmount);
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
        ImGui::Separator();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Back to Dashboard", ImVec2(200, 0))) {
        currentScreen = SCREEN_DASHBOARD;
    }

    ImGui::End();
}

void RenderDashboard()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Dashboard", NULL, 
                 ImGuiWindowFlags_NoMove | 
                 ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoCollapse | 
                 ImGuiWindowFlags_NoTitleBar);

    static char invitationsSent[50] = "";
    static char rsvpsReceived[50] = "";
    static char budgetBreakdown1[100] = "";
    static char budgetBreakdown2[100] = "";
    static char budgetBreakdown3[100] = "";
    static bool budgetInputOption1 = true; // true = counter, false = direct input
    static float budgetSpentOption2 = 0.0f;
    static float manualStressLevel = 2.0f; // 0=not stressed, 1=mild, 2=neutral, 3=very stressed, 4=extremely stressed
    
    // Motivational messages based on progress
    const char *motivationalMessages[] = {
        "You've got this!",
        "Great start! Keep going!",
        "Halfway there! You're doing amazing!",
        "Almost done! Don't give up!",
        "Fantastic! You crushed it!",
        "Remember, every small step counts!",
        "You're stronger than your challenges!",
        "Focus on one task at a time!",
        "Progress over perfection!",
        "Believe in yourself! You can do it!"
    };

    // Create tabs for Dashboard and Calendar
    ImGui::BeginTabBar("DashboardTabs");
    
    if (ImGui::BeginTabItem("Event Planning"))
    {
        ImGui::Columns(2, "dashboard_columns", false);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.5f - 20);

        // ===== LEFT COLUMN =====

        // Event Details Card (Yellow)
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.93f, 0.96f, 1.0f));
        ImGui::BeginChild("event_details_card", ImVec2(0, 200), true, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.84f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.84f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.84f, 0.0f, 1.0f));
    if (ImGui::Button("Event Details", ImVec2(-1, 0))) {}
    ImGui::PopStyleColor(3);
    
    ImGui::Separator();
    ImGui::Text("Name:");
    ImGui::InputText("##event_name", selectedEventName, IM_ARRAYSIZE(selectedEventName));
    if (!selectedOptionName.empty())
    {
        ImGui::Text("Selected Option: %s", selectedOptionName.c_str());
    }
    ImGui::Text("Date:");
    ImGui::InputText("##event_date", selectedEventDate, IM_ARRAYSIZE(selectedEventDate));

    static bool calendarDayToggles[7] = {};
    static const char *calendarDayLabels[7] = {
        "Mon##cal0", "Tue##cal1", "Wed##cal2", "Thu##cal3", "Fri##cal4", "Sat##cal5", "Sun##cal6"
    };

    int daysLeft = DaysUntilEvent(selectedEventDate);
    if (daysLeft >= 0)
    {
        if (daysLeft == 0)
            ImGui::Text("Days left until event: Today");
        else if (daysLeft == 1)
            ImGui::Text("Days left until event: 1 day");
        else
            ImGui::Text("Days left until event: %d days", daysLeft);
    }
    else
    {
        ImGui::Text("Days left until event: Enter a valid date");
    }

    ImGui::Text("Calendar:");
    for (int i = 0; i < 7; i++)
    {
        if (i > 0)
            ImGui::SameLine();
        ImGui::Checkbox(calendarDayLabels[i], &calendarDayToggles[i]);
    }
    ImGui::NewLine();
    ImGui::NewLine();

    ImGui::Text("Location/Venue:");
    ImGui::InputText("##event_location", selectedEventLocation, IM_ARRAYSIZE(selectedEventLocation));
    ImGui::Text("Number of Guests:");
    ImGui::InputText("##event_guests", selectedEventGuests, IM_ARRAYSIZE(selectedEventGuests));
    
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // To-Do List Card (Green)
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.93f, 0.96f, 1.0f));
    ImGui::BeginChild("todo_list_card", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.196f, 0.804f, 0.196f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.196f, 0.804f, 0.196f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.196f, 0.804f, 0.196f, 1.0f));
    if (ImGui::Button("To-Do List", ImVec2(-1, 0))) {}
    ImGui::PopStyleColor(3);
    
    ImGui::Separator();

    int completed = 0;
    if (currentChecklistTasks && !currentChecklistTasks->empty())
    {
        if (taskDone.size() != currentChecklistTasks->size())
        {
            taskDone = std::vector<bool>(currentChecklistTasks->size(), false);
            taskDaysLeft = std::vector<int>(currentChecklistTasks->size(), 7);
        }

        for (size_t i = 0; i < currentChecklistTasks->size(); i++)
        {
            bool done = taskDone[i];
            if (ImGui::Checkbox((*currentChecklistTasks)[i], &done))
                taskDone[i] = done;
            if (taskDone[i])
                completed++;
        }
    }
    else if (currentDynamicTasks && !currentDynamicTasks->empty())
    {
        if (taskDone.size() != currentDynamicTasks->size())
        {
            taskDone = std::vector<bool>(currentDynamicTasks->size(), false);
            taskDaysLeft = std::vector<int>(currentDynamicTasks->size(), 7);
        }

        for (size_t i = 0; i < currentDynamicTasks->size(); i++)
        {
            bool done = taskDone[i];
            if (ImGui::Checkbox((*currentDynamicTasks)[i].c_str(), &done))
                taskDone[i] = done;
            if (taskDone[i])
                completed++;
        }
    }
    else if (!taskDone.empty())
    {
        for (size_t i = 0; i < taskDone.size(); i++)
        {
            bool done = taskDone[i];
            std::string label = std::to_string(i + 1);
            if (ImGui::Checkbox(label.c_str(), &done))
                taskDone[i] = done;
            if (taskDone[i])
                completed++;
        }
    }
    else
    {
        ImGui::Text("No checklist tasks selected yet. Pick an option to generate tasks.");
    }

    ImGui::Text("Completed tasks: %d / %zu", completed, taskDone.size());
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::NextColumn();

    // ===== RIGHT COLUMN =====

    // Guest List Card (Teal)
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.93f, 0.96f, 1.0f));
    ImGui::BeginChild("guest_list_card", ImVec2(0, 150), true, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.808f, 0.816f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.808f, 0.816f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.808f, 0.816f, 1.0f));
    if (ImGui::Button("Guest List", ImVec2(-1, 0))) {}
    ImGui::PopStyleColor(3);
    
    ImGui::Separator();
    ImGui::Text("Total Guests: %s", selectedEventGuests);
    ImGui::Text("Invitations Sent:");
    ImGui::InputText("##invitations_sent", invitationsSent, IM_ARRAYSIZE(invitationsSent));
    ImGui::Text("RSVPs Received:");
    ImGui::InputText("##rsvps_received", rsvpsReceived, IM_ARRAYSIZE(rsvpsReceived));
    
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // Budget Card (Purple)
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.93f, 0.96f, 1.0f));
    ImGui::BeginChild("budget_card", ImVec2(0, 200), true, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.855f, 0.439f, 0.839f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.855f, 0.439f, 0.839f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.855f, 0.439f, 0.839f, 1.0f));
    if (ImGui::Button("Budget", ImVec2(-1, 0))) {}
    ImGui::PopStyleColor(3);
    
    ImGui::Separator();
    
    float displayBudget = totalBudget;
    if (selectedEvent == EventType::Party && partyBudget > 0.0f)
        displayBudget = partyBudget;
    else if (selectedEvent == EventType::Wedding && weddingBudget > 0.0f)
        displayBudget = weddingBudget;
    else if (selectedEvent == EventType::Birthday && birthdayBudget > 0.0f)
        displayBudget = birthdayBudget;
    else if (selectedEvent == EventType::Engagement && engagementBudget > 0.0f)
        displayBudget = engagementBudget;
    else if (selectedEvent == EventType::Conference && conferenceBudget > 0.0f)
        displayBudget = conferenceBudget;
    
    ImGui::Text("Total Budget: %.0f", displayBudget);
    ImGui::Text("Spent: %.0f", spentBudget);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Budget Input Methods:");
    
    if (ImGui::RadioButton("Option 1: Counter Button", budgetInputOption1))
        budgetInputOption1 = true;
    
    ImGui::SameLine();
    
    if (ImGui::RadioButton("Option 2: Direct Input", !budgetInputOption1))
        budgetInputOption1 = false;
    
    ImGui::Spacing();
    
    if (budgetInputOption1)
    {
        ImGui::InputFloat("Update Spent Amount##option1", &spentBudget, 100.0f, 1000.0f, "%.0f");
    }
    else
    {
        ImGui::InputFloat("Spent Amount##option2", &budgetSpentOption2, 100.0f, 1000.0f, "%.0f");
        ImGui::Spacing();
        if (ImGui::Button("Update Budget from Option 2", ImVec2(-1, 0)))
        {
            spentBudget = budgetSpentOption2;
        }
    }
    
    ImGui::Text("Remaining: %.0f", displayBudget - spentBudget);
    ImGui::Separator();
    
    if (ImGui::Button("Budget System", ImVec2(-1, 0))) {
        currentScreen = SCREEN_BUDGET_SYSTEM;
    }
    ImGui::Spacing();
    ImGui::Text("Click to see detailed budget allocation suggestions");
    
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // Progress Card (Blue)
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.93f, 0.96f, 1.0f));
    ImGui::BeginChild("progress_card", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
    if (ImGui::Button("Progress Trackers & Stress Level", ImVec2(-1, 0))) {}
    ImGui::PopStyleColor(3);

    ImGui::Separator();

    // Tasks progress
    int totalTasks = taskDone.size();
    int completedTasks = 0;
    for (bool done : taskDone) if (done) completedTasks++;
    float taskProgress = totalTasks > 0 ? (float)completedTasks / totalTasks * 100.0f : 0.0f;
    ImGui::Text("Tasks Progress: %.1f%% (%d/%d)", taskProgress, completedTasks, totalTasks);
    ImGui::ProgressBar(taskProgress / 100.0f);

    ImGui::Spacing();

    // Budget progress
    float budgetProgress = displayBudget > 0 ? spentBudget / displayBudget * 100.0f : 0.0f;
    ImGui::Text("Budget Progress: %.1f%% (%.0f/%.0f)", budgetProgress, spentBudget, displayBudget);
    ImGui::ProgressBar(budgetProgress / 100.0f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Automatic Stress Level based on Task Progress
    float autoStressLevel = (100.0f - taskProgress) / 25.0f; // Inverse relationship: high progress = low stress
    if (autoStressLevel > 4.0f) autoStressLevel = 4.0f;
    if (autoStressLevel < 0.0f) autoStressLevel = 0.0f;

    ImGui::Text("Task-Based Stress Level:");
    const char *autoStressLabels[] = {"Very low", "Low", "Moderate", "High", "Very high"};
    ImGui::Text("%s", autoStressLabels[(int)autoStressLevel]);
    ImGui::SameLine();
    float stressProgress = autoStressLevel / 4.0f;
    ImGui::ProgressBar(stressProgress, ImVec2(200, 0));

    ImGui::Spacing();
    ImGui::Spacing();

    // Manual Stress Level with Slider
    ImGui::Text("Your Stress Level:");
    const char *stressLabels[] = {"Not Stressed", "Mild", "Neutral", "Very Stressed", "Extremely Stressed"};
    
    ImGui::Text("%s", stressLabels[(int)manualStressLevel]);
    ImGui::SliderFloat("Stress Level##slider", &manualStressLevel, 0.0f, 4.0f, "");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Motivational Message
    int messageIndex = (int)((completedTasks * 73) % 10); // Different message based on completed tasks
    ImGui::Text("Tip: %s", motivationalMessages[messageIndex]);

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Columns(1);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::EndTabItem();
    }
    
    if (ImGui::BeginTabItem("Weekly Calendar"))
    {
        // Calendar grid rendering
        ImVec2 windowSize = ImGui::GetContentRegionAvail();
        float cellWidth = (windowSize.x - 80.0f) / 7.0f;
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.9f), "Weekly Calendar View");
        ImGui::SameLine(windowSize.x - 150.0f);

        if (ImGui::Button("+ New Task##Dashboard", ImVec2(140.0f, 0)))
        {
            calendarState.showCreateModal = true;
            calendarState.modalDayOfWeek = 0;
            calendarState.modalStartHour = 9;
            calendarState.modalStartMinute = 0;
            calendarState.modalEndHour = 10;
            calendarState.modalEndMinute = 0;
            ImGui::OpenPopup("Create Task");
        }

        ImGui::Spacing();

        // Get current week dates
        time_t now = time(nullptr);
        struct tm *timeinfo = localtime(&now);
        int currentDayOfWeek = timeinfo->tm_wday;

        // Calculate Monday of this week
        int daysFromMonday = (currentDayOfWeek + 6) % 7;
        time_t monday = now - (daysFromMonday * 24 * 60 * 60);
        struct tm *mondayInfo = localtime(&monday);

        // Display week date range
        char dateRange[64];
        strftime(dateRange, sizeof(dateRange), "%b %d", mondayInfo);
        time_t sunday = monday + (6 * 24 * 60 * 60);
        struct tm *sundayInfo = localtime(&sunday);
        char sundayStr[32];
        strftime(sundayStr, sizeof(sundayStr), "%b %d, %Y", sundayInfo);

        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 0.8f), "Week of %s - %s", dateRange, sundayStr);

        ImGui::Spacing();
        
        // Render calendar grid
        RenderCalendarGrid(calendarState, cellWidth);
        
        // Task modal
        RenderCalendarTaskModal(calendarState);

        ImGui::EndTabItem();
    }
    
    ImGui::EndTabBar();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Back to Event Details", ImVec2(200, 40)))
    {
        currentScreen = SCREEN_EVENT_DETAILS;
    }

    ImGui::End();
}
// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------

int main()
{

    if (!glfwInit())
        return 1;

    // Initialize budget allocation data
    InitializeBudgetAllocations();

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

    ImGuiIO &io = ImGui::GetIO();
    ImFont *fontDefault = io.Fonts->AddFontDefault();
    ImFont *fontRegular = io.Fonts->AddFontFromFileTTF("/Library/Fonts/Arial Unicode.ttf", 24.0f);
    if (!fontRegular)
        fontRegular = io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/Helvetica.ttc", 24.0f);
    io.FontGlobalScale = 1.15f;

    if (!fontRegular)
    {
        fontRegular = fontDefault;
        printf("Warning: failed to load Arial Unicode.ttf or Helvetica.ttc, using default fallback.\n");
    }

    io.FontDefault = fontRegular;

    if (!fontDefault)
        printf("Warning: failed to load default ImGui font.\n");

    SetPinkTheme();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    (void)io;

    backgroundTexture = LoadTexture("/Users/ahmedabdelbadie/Downloads/balloon-desktop-wallpaper_862994-352504 copy.png");
    if (backgroundTexture == 0)
        printf("Warning: could not load login background image.\n");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        float window_width = io.DisplaySize.x;

        // ================= LOGIN =================

        if (currentScreen == SCREEN_LOGIN)
        {
            if (backgroundTexture != 0)
            {
                ImGui::GetBackgroundDrawList()->AddImage(
                    reinterpret_cast<void *>(
                        static_cast<intptr_t>(backgroundTexture)),
                    ImVec2(0, 0),
                    io.DisplaySize,
                    ImVec2(0, 0),
                    ImVec2(1, 1));
            }

            ImGui::GetBackgroundDrawList()->AddRectFilled(
                ImVec2(0, 0),
                io.DisplaySize,
                ImColor(0, 0, 0, 60));

            float cardWidth = 520.0f;
            float field_width = 480.0f;

            ImGui::SetNextWindowPos(
                ImVec2(window_width * 0.5f, io.DisplaySize.y * 0.5f),
                ImGuiCond_Always,
                ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSizeConstraints(
                ImVec2(cardWidth, 0.0f),
                ImVec2(cardWidth, 1e6f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.25f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 0.12f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.18f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.74f, 0.82f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.84f, 0.88f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.93f, 0.64f, 0.72f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.14f, 0.14f, 0.14f, 1.0f));

            ImGui::Begin("Welcome to Eventopia", NULL,
                         ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoTitleBar);

            static char username[50] = "";
            static char password[50] = "";
            static std::string loginMessage = "";

            ImGui::SetWindowFontScale(1.35f);
            ImGui::SetCursorPosX((cardWidth - ImGui::CalcTextSize("Sign In").x) * 0.5f);
            ImGui::Text("Sign In");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Spacing();

            ImGui::SetCursorPosX((cardWidth - field_width) * 0.5f);
            ImGui::PushItemWidth(field_width);
            ImGui::InputTextWithHint("##Username", "Username", username, 50);
            ImGui::PopItemWidth();

            ImGui::Spacing();
            ImGui::SetCursorPosX((cardWidth - field_width) * 0.5f);
            ImGui::PushItemWidth(field_width);
            ImGui::InputTextWithHint("##Password", "Password", password, 50, ImGuiInputTextFlags_Password);
            ImGui::PopItemWidth();

            ImGui::Spacing();
            bool canLogin = strlen(username) > 0 && strlen(password) > 0;
            ImGui::SetCursorPosX((cardWidth - field_width) * 0.5f);

            if (!canLogin)
                ImGui::BeginDisabled();
            if (ImGui::Button("Login", ImVec2(field_width, 60)))
            {
                if (accountExists(username, password))
                {
                    loginMessage = "Login successful!";
                    std::ifstream file(std::string(username) + "_event.txt");
                    if (file.is_open())
                    {
                        int e;
                        file >> e;
                        selectedEvent = static_cast<EventType>(e);
                        file.close();
                    }
                    std::ofstream outfile(std::string(username) + "_event.txt");
                    outfile << static_cast<int>(selectedEvent) << std::endl;
                    outfile.close();
                    currentScreen = SCREEN_MAIN_MENU;
                }
                else
                {
                    loginMessage = "Incorrect username or password.";
                }
            }
            if (!canLogin)
                ImGui::EndDisabled();

            ImGui::Spacing();
            ImGui::SetCursorPosX(24.0f);
            ImGui::TextColored(ImVec4(0.45f, 0.45f, 0.45f, 1.0f), "Forget Password");
            ImGui::SameLine();
            ImGui::SetCursorPosX(cardWidth - ImGui::CalcTextSize("Sign Up").x - 24.0f);
            ImGui::TextColored(ImVec4(0.45f, 0.45f, 0.45f, 1.0f), "Sign Up");
            if (ImGui::IsItemClicked())
                currentScreen = SCREEN_CREATE_ACCOUNT;

            ImGui::Spacing();
            ImGui::Text("%s", loginMessage.c_str());

            ImVec2 windowSize = ImGui::GetWindowSize();
            ImGui::SetWindowPos(ImVec2(
                (window_width - windowSize.x) * 0.5f,
                (io.DisplaySize.y - windowSize.y) * 0.5f));

            ImGui::End();

            ImGui::PopStyleColor(8);
            ImGui::PopStyleVar(3);
        }

        // ================= CREATE ACCOUNT =================

        if (currentScreen == SCREEN_CREATE_ACCOUNT)
        {
            if (backgroundTexture != 0)
            {
                ImGui::GetBackgroundDrawList()->AddImage(
                    reinterpret_cast<void *>(
                        static_cast<intptr_t>(backgroundTexture)),
                    ImVec2(0, 0),
                    io.DisplaySize,
                    ImVec2(0, 0),
                    ImVec2(1, 1));
            }

            ImGui::GetBackgroundDrawList()->AddRectFilled(
                ImVec2(0, 0),
                io.DisplaySize,
                ImColor(0, 0, 0, 60));

            float cardWidth = 520.0f;
            float field_width = 480.0f;

            ImGui::SetNextWindowPos(
                ImVec2(window_width * 0.5f, io.DisplaySize.y * 0.5f),
                ImGuiCond_Always,
                ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSizeConstraints(
                ImVec2(cardWidth, 0.0f),
                ImVec2(cardWidth, 1e6f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.25f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 0.12f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.18f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.74f, 0.82f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.84f, 0.88f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.93f, 0.64f, 0.72f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.14f, 0.14f, 0.14f, 1.0f));

            ImGui::Begin("Create Account", NULL,
                         ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoTitleBar);

            static char newUsername[50] = "";
            static char newPassword[50] = "";

            static std::string message = "";

            ImGui::SetWindowFontScale(1.35f);
            ImGui::SetCursorPosX((cardWidth - ImGui::CalcTextSize("Create your account").x) * 0.5f);
            ImGui::Text("Create your account");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Spacing();

            ImGui::SetCursorPosX((cardWidth - field_width) * 0.5f);
            ImGui::PushItemWidth(field_width);
            ImGui::InputTextWithHint("##NewUsername", "Username", newUsername, 50);
            ImGui::PopItemWidth();

            ImGui::Spacing();

            ImGui::SetCursorPosX((cardWidth - field_width) * 0.5f);
            ImGui::PushItemWidth(field_width);
            ImGui::InputTextWithHint("##NewPassword", "Password", newPassword, 50, ImGuiInputTextFlags_Password);
            ImGui::PopItemWidth();

            ImGui::Spacing();

            bool canCreate =
                strlen(newUsername) > 0 &&
                strlen(newPassword) > 0;

            ImGui::SetCursorPosX((cardWidth - field_width) * 0.5f);

            if (!canCreate)
                ImGui::BeginDisabled();

            if (ImGui::Button(
                    "Create Account",
                    ImVec2(field_width, 60)))
            {
                saveAccount(newUsername, newPassword);

                message =
                    "Account created successfully!";

                strcpy(newUsername, "");
                strcpy(newPassword, "");

                currentScreen = SCREEN_MAIN_MENU;
            }

            if (!canCreate)
                ImGui::EndDisabled();

            ImGui::Spacing();

            ImGui::SetCursorPosX((cardWidth - field_width) * 0.5f);

            if (ImGui::Button(
                    "Back to Home",
                    ImVec2(field_width, 60)))
            {
                syncAndSaveCurrentEvent();
                currentScreen = SCREEN_MAIN_MENU;
            }

            ImGui::Spacing();

            ImGui::Text("%s", message.c_str());

            ImVec2 windowSize = ImGui::GetWindowSize();
            ImGui::SetWindowPos(ImVec2(
                (window_width - windowSize.x) * 0.5f,
                (io.DisplaySize.y - windowSize.y) * 0.5f));

            ImGui::End();

            ImGui::PopStyleColor(8);
            ImGui::PopStyleVar(3);
        }

        // ================= MAIN MENU =================

        else if (currentScreen == SCREEN_MAIN_MENU)
        {
            loadEvents(); // Load saved events
            
            ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - 500) / 2, (ImGui::GetIO().DisplaySize.y - 400) / 2));
            ImGui::SetNextWindowSize(ImVec2(500, 400));
            ImGui::Begin("Home Screen", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.6f, 0.8f, 1.0f)); // baby pink
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.7f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.7f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            ImGui::PushFont(fontRegular);
            ImGui::SetWindowFontScale(1.5f);
            ImGui::Text("Event Planning App");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Spacing();
            
            if (ImGui::Button("Create Event", ImVec2(450, 60)))
            {
                currentEvent = nullptr;
                selectedEvent = EventType::None;
                currentScreen = SCREEN_EVENT_SELECTION;
            }
            
            if (ImGui::Button("Continue Event Planning", ImVec2(450, 60)))
            {
                if (!savedEvents.empty())
                {
                    currentScreen = SCREEN_EVENT_LIST;
                }
                else
                {
                    ImGui::Text("No existing events found.");
                }
            }
            
            ImGui::PopFont();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
            ImGui::End();
        }

        // ================= EVENT SELECTION =================

        else if (currentScreen == SCREEN_EVENT_SELECTION)
        {
            ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - 400) / 2, (ImGui::GetIO().DisplaySize.y - 400) / 2));
            ImGui::SetNextWindowSize(ImVec2(400, 400));
            ImGui::Begin("Select Event Type", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.6f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.7f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.7f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            ImGui::PushFont(fontRegular);
            ImGui::SetWindowFontScale(1.2f);
            ImGui::Text("Select Event Type:");
            ImGui::SetWindowFontScale(1.0f);
            if (ImGui::Button("Party", ImVec2(350, 40)))
            {
                selectedEvent = EventType::Party;
                ResetEventOptionState();
                currentEvent = nullptr;
                currentChecklistTasks = nullptr;
                currentDynamicTasks = nullptr;
                taskDone.clear();
                taskDaysLeft.clear();
                currentScreen = SCREEN_EVENT_OPTIONS;
            }
            if (ImGui::Button("Wedding", ImVec2(350, 40)))
            {
                selectedEvent = EventType::Wedding;
                ResetEventOptionState();
                currentEvent = nullptr;
                currentChecklistTasks = nullptr;
                currentDynamicTasks = nullptr;
                taskDone.clear();
                taskDaysLeft.clear();
                currentScreen = SCREEN_EVENT_OPTIONS;
            }
            if (ImGui::Button("Birthday", ImVec2(350, 40)))
            {
                selectedEvent = EventType::Birthday;
                ResetEventOptionState();
                currentEvent = nullptr;
                currentChecklistTasks = nullptr;
                currentDynamicTasks = nullptr;
                taskDone.clear();
                taskDaysLeft.clear();
                currentScreen = SCREEN_EVENT_OPTIONS;
            }
            if (ImGui::Button("Conference", ImVec2(350, 40)))
            {
                selectedEvent = EventType::Conference;
                ResetEventOptionState();
                currentEvent = nullptr;
                currentChecklistTasks = nullptr;
                currentDynamicTasks = nullptr;
                taskDone.clear();
                taskDaysLeft.clear();
                currentScreen = SCREEN_EVENT_OPTIONS;
            }
            if (ImGui::Button("Engagement", ImVec2(350, 40)))
            {
                selectedEvent = EventType::Engagement;
                ResetEventOptionState();
                currentEvent = nullptr;
                currentChecklistTasks = nullptr;
                currentDynamicTasks = nullptr;
                taskDone.clear();
                taskDaysLeft.clear();
                currentScreen = SCREEN_EVENT_OPTIONS;
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Back to Home", ImVec2(160.0f, 35.0f)))
            {
                currentScreen = SCREEN_MAIN_MENU;
            }

            ImGui::PopFont();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
            ImGui::End();
        }

        // ================= EVENT OPTIONS =================

        else if (currentScreen == SCREEN_EVENT_OPTIONS)
        {
            ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - 400) / 2, (ImGui::GetIO().DisplaySize.y - 400) / 2));
            ImGui::SetNextWindowSize(ImVec2(400, 400));
            ImGui::Begin("Select Event Option", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.6f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.7f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.7f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            ImGui::PushFont(fontRegular);
            ImGui::SetWindowFontScale(1.2f);
            ImGui::Text("Select Event Option:");
            ImGui::SetWindowFontScale(1.0f);

            if (selectedEvent == EventType::Birthday)
            {
                if (ImGui::Button("Basic", ImVec2(350, 40)))
                {
                    CreateEventWithOption(1, 5000.0f, "New Birthday Event");
                }
                if (ImGui::Button("Mid", ImVec2(350, 40)))
                {
                    CreateEventWithOption(2, 10000.0f, "New Birthday Event");
                }
                if (ImGui::Button("Premium", ImVec2(350, 40)))
                {
                    CreateEventWithOption(3, 15000.0f, "New Birthday Event");
                }
                if (ImGui::Button("Deluxe", ImVec2(350, 40)))
                {
                    CreateEventWithOption(4, 20000.0f, "New Birthday Event");
                }
                if (ImGui::Button("Luxury", ImVec2(350, 40)))
                {
                    CreateEventWithOption(5, 25000.0f, "New Birthday Event");
                }
                if (ImGui::Button("Ultra Luxury", ImVec2(350, 40)))
                {
                    CreateEventWithOption(6, 30000.0f, "New Birthday Event");
                }
            }
            else if (selectedEvent == EventType::Wedding)
            {
                if (ImGui::Button("Basic", ImVec2(350, 40)))
                {
                    CreateEventWithOption(1, 20000.0f, "New Wedding Event");
                }
                if (ImGui::Button("Mid", ImVec2(350, 40)))
                {
                    CreateEventWithOption(2, 50000.0f, "New Wedding Event");
                }
                if (ImGui::Button("Premium", ImVec2(350, 40)))
                {
                    CreateEventWithOption(3, 80000.0f, "New Wedding Event");
                }
                if (ImGui::Button("Deluxe", ImVec2(350, 40)))
                {
                    CreateEventWithOption(4, 100000.0f, "New Wedding Event");
                }
                if (ImGui::Button("Luxury", ImVec2(350, 40)))
                {
                    CreateEventWithOption(5, 150000.0f, "New Wedding Event");
                }
                if (ImGui::Button("Elite Luxury", ImVec2(350, 40)))
                {
                    CreateEventWithOption(6, 200000.0f, "New Wedding Event");
                }
            }
            else if (selectedEvent == EventType::Party)
            {
                if (ImGui::Button("Basic", ImVec2(350, 40)))
                {
                    CreateEventWithOption(1, 3000.0f, "New Party Event");
                }
                if (ImGui::Button("Mid", ImVec2(350, 40)))
                {
                    CreateEventWithOption(2, 7000.0f, "New Party Event");
                }
                if (ImGui::Button("Premium", ImVec2(350, 40)))
                {
                    CreateEventWithOption(3, 12000.0f, "New Party Event");
                }
                if (ImGui::Button("Deluxe", ImVec2(350, 40)))
                {
                    CreateEventWithOption(4, 18000.0f, "New Party Event");
                }
                if (ImGui::Button("Luxury", ImVec2(350, 40)))
                {
                    CreateEventWithOption(5, 25000.0f, "New Party Event");
                }
            }
            else if (selectedEvent == EventType::Conference)
            {
                if (ImGui::Button("Basic", ImVec2(350, 40)))
                {
                    CreateEventWithOption(1, 10000.0f, "New Conference Event");
                }
                if (ImGui::Button("Mid", ImVec2(350, 40)))
                {
                    CreateEventWithOption(2, 20000.0f, "New Conference Event");
                }
                if (ImGui::Button("Premium", ImVec2(350, 40)))
                {
                    CreateEventWithOption(3, 30000.0f, "New Conference Event");
                }
                if (ImGui::Button("Deluxe", ImVec2(350, 40)))
                {
                    CreateEventWithOption(4, 40000.0f, "New Conference Event");
                }
                if (ImGui::Button("Luxury", ImVec2(350, 40)))
                {
                    CreateEventWithOption(5, 50000.0f, "New Conference Event");
                }
                if (ImGui::Button("Elite", ImVec2(350, 40)))
                {
                    CreateEventWithOption(6, 60000.0f, "New Conference Event");
                }
            }
            else if (selectedEvent == EventType::Engagement)
            {
                if (ImGui::Button("Basic", ImVec2(350, 40)))
                {
                    CreateEventWithOption(1, 5000.0f, "New Engagement Event");
                }
                if (ImGui::Button("Mid", ImVec2(350, 40)))
                {
                    CreateEventWithOption(2, 10000.0f, "New Engagement Event");
                }
                if (ImGui::Button("Premium", ImVec2(350, 40)))
                {
                    CreateEventWithOption(3, 15000.0f, "New Engagement Event");
                }
                if (ImGui::Button("Deluxe", ImVec2(350, 40)))
                {
                    CreateEventWithOption(4, 20000.0f, "New Engagement Event");
                }
                if (ImGui::Button("Luxury", ImVec2(350, 40)))
                {
                    CreateEventWithOption(5, 25000.0f, "New Engagement Event");
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Back to Event Type", ImVec2(200.0f, 35.0f)))
            {
                currentScreen = SCREEN_EVENT_TYPE;
            }

            ImGui::PopFont();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
            ImGui::End();
        }

        // ================= EVENT LIST =================

        else if (currentScreen == SCREEN_EVENT_LIST)
        {
            ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - 600) / 2, (ImGui::GetIO().DisplaySize.y - 500) / 2));
            ImGui::SetNextWindowSize(ImVec2(600, 500));
            ImGui::Begin("Saved Events", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.6f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.7f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.7f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            ImGui::PushFont(fontRegular);
            ImGui::SetWindowFontScale(1.2f);
            ImGui::Text("Select an event to continue planning:");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Separator();
            
            for (size_t i = 0; i < savedEvents.size(); ++i) {
                const auto& event = savedEvents[i];
                std::string buttonText = event.name + " (" + event.date + ")";
                if (ImGui::Button(buttonText.c_str(), ImVec2(550, 40))) {
                    currentEvent = &savedEvents[i];
                    selectedEvent = event.type;
                    
                    // Set selected option
                    if (event.type == EventType::Birthday) {
                        selectedBirthdayOption = static_cast<BirthdayOption>(event.selectedOption);
                    } else if (event.type == EventType::Wedding) {
                        selectedWeddingOption = static_cast<WeddingOption>(event.selectedOption);
                    } else if (event.type == EventType::Party) {
                        selectedPartyOption = static_cast<PartyOption>(event.selectedOption);
                    } else if (event.type == EventType::Conference) {
                        selectedConferenceOption = static_cast<ConferenceOption>(event.selectedOption);
                    } else if (event.type == EventType::Engagement) {
                        selectedEngagementOption = static_cast<EngagementOption>(event.selectedOption);
                    }
                    
                    // Restore saved option budget range
                    SetOptionBudgetRange();
                    
                    // Set dashboard variables
                    strcpy(selectedEventName, event.name.c_str());
                    strcpy(selectedEventDate, event.date.c_str());
                    strcpy(selectedEventLocation, event.location.c_str());
                    strcpy(selectedEventGuests, event.guestList.c_str());
                    totalBudget = event.budget;
                    spentBudget = 0.0f;
                    
                    // Set type-specific budget
                    if (event.type == EventType::Birthday) {
                        birthdayBudget = event.budget;
                    } else if (event.type == EventType::Wedding) {
                        weddingBudget = event.budget;
                    } else if (event.type == EventType::Party) {
                        partyBudget = event.budget;
                    } else if (event.type == EventType::Engagement) {
                        engagementBudget = event.budget;
                    } else if (event.type == EventType::Conference) {
                        conferenceBudget = event.budget;
                    }
                    
                    // Set up tasks
                    if (!event.tasks.empty()) {
                        currentEventTasks = event.tasks;
                        UpdateDynamicTasks(currentEventTasks);
                        taskDone = event.taskCompleted;
                    }
                    
                    currentScreen = SCREEN_DASHBOARD;
                }
            }
            
            ImGui::Separator();
            if (ImGui::Button("Back to Home", ImVec2(550, 40))) {
                currentScreen = SCREEN_MAIN_MENU;
            }
            
            ImGui::PopFont();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
            ImGui::End();
        }

        // ================= EVENT BASIC DETAILS =================

        else if (currentScreen == SCREEN_EVENT_BASIC_DETAILS)
        {
            static char eventName[100] = "";
            static char eventDate[50] = "";
            static char eventLocation[100] = "";
            static float eventBudget = 0.0f;
            static char eventGuests[200] = "";
            static char eventNotes[500] = "";
            
            ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - 500) / 2, (ImGui::GetIO().DisplaySize.y - 600) / 2));
            ImGui::SetNextWindowSize(ImVec2(500, 600));
            ImGui::Begin("Event Details", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.6f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.7f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.7f, 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            ImGui::PushFont(fontRegular);
            ImGui::SetWindowFontScale(1.2f);
            ImGui::Text("Enter Event Details:");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Separator();
            
            ImGui::Text("Event Name:");
            ImGui::InputText("##name", eventName, IM_ARRAYSIZE(eventName));
            
            ImGui::Text("Date:");
            ImGui::InputText("##date", eventDate, IM_ARRAYSIZE(eventDate));
            
            ImGui::Text("Location:");
            ImGui::InputText("##location", eventLocation, IM_ARRAYSIZE(eventLocation));
            
            ImGui::Text("Budget:");
            ImGui::InputFloat("##budget", &eventBudget, 100.0f, 1000.0f, "%.0f");
            
            ImGui::Text("Guest List:");
            ImGui::InputTextMultiline("##guests", eventGuests, IM_ARRAYSIZE(eventGuests), ImVec2(-1, 60));
            
            ImGui::Text("Notes:");
            ImGui::InputTextMultiline("##notes", eventNotes, IM_ARRAYSIZE(eventNotes), ImVec2(-1, 80));
            
            ImGui::Separator();
            bool canCreate = strlen(eventName) > 0 && strlen(eventDate) > 0;
            if (!canCreate) {
                ImGui::TextColored(ImVec4(1,0,0,1), "Event name and date are required.");
            }
            if (ImGui::Button("Create Event", ImVec2(450, 50)) && canCreate)
            {
                // Create new event
                Event newEvent;
                newEvent.name = eventName;
                newEvent.date = eventDate;
                newEvent.location = eventLocation;
                newEvent.budget = eventBudget;
                newEvent.guestList = eventGuests;
                newEvent.notes = eventNotes;
                newEvent.type = selectedEvent;
                
                // Set selectedOption
                if (selectedEvent == EventType::Birthday) {
                    newEvent.selectedOption = static_cast<int>(selectedBirthdayOption);
                } else if (selectedEvent == EventType::Wedding) {
                    newEvent.selectedOption = static_cast<int>(selectedWeddingOption);
                } else if (selectedEvent == EventType::Party) {
                    newEvent.selectedOption = static_cast<int>(selectedPartyOption);
                } else if (selectedEvent == EventType::Conference) {
                    newEvent.selectedOption = static_cast<int>(selectedConferenceOption);
                } else if (selectedEvent == EventType::Engagement) {
                    newEvent.selectedOption = static_cast<int>(selectedEngagementOption);
                } else {
                    newEvent.selectedOption = 0;
                }
                
                // Add tasks based on event type and option
                if (selectedEvent == EventType::Birthday) {
                    if (selectedBirthdayOption == BirthdayOption::Option1) {
                        newEvent.tasks.assign(birthdayOption1Tasks.begin(), birthdayOption1Tasks.end());
                    } else if (selectedBirthdayOption == BirthdayOption::Option2) {
                        newEvent.tasks.assign(birthdayOption2Tasks.begin(), birthdayOption2Tasks.end());
                    } else if (selectedBirthdayOption == BirthdayOption::Option3) {
                        newEvent.tasks.assign(birthdayOption3Tasks.begin(), birthdayOption3Tasks.end());
                    } else if (selectedBirthdayOption == BirthdayOption::Option4) {
                        newEvent.tasks.assign(birthdayOption4Tasks.begin(), birthdayOption4Tasks.end());
                    } else if (selectedBirthdayOption == BirthdayOption::Option5) {
                        newEvent.tasks.assign(birthdayOption5Tasks.begin(), birthdayOption5Tasks.end());
                    } else if (selectedBirthdayOption == BirthdayOption::Option6) {
                        newEvent.tasks.assign(birthdayOption6Tasks.begin(), birthdayOption6Tasks.end());
                    }
                } else if (selectedEvent == EventType::Wedding) {
                    if (selectedWeddingOption == WeddingOption::Option1) {
                        newEvent.tasks.assign(weddingOption1Tasks.begin(), weddingOption1Tasks.end());
                    } else if (selectedWeddingOption == WeddingOption::Option2) {
                        newEvent.tasks.assign(weddingOption2Tasks.begin(), weddingOption2Tasks.end());
                    } else if (selectedWeddingOption == WeddingOption::Option3) {
                        newEvent.tasks.assign(weddingOption3Tasks.begin(), weddingOption3Tasks.end());
                    } else if (selectedWeddingOption == WeddingOption::Option4) {
                        newEvent.tasks.assign(weddingOption4Tasks.begin(), weddingOption4Tasks.end());
                    } else if (selectedWeddingOption == WeddingOption::Option5) {
                        newEvent.tasks.assign(weddingOption5Tasks.begin(), weddingOption5Tasks.end());
                    } else if (selectedWeddingOption == WeddingOption::Option6) {
                        newEvent.tasks.assign(weddingOption6Tasks.begin(), weddingOption6Tasks.end());
                    }
                } else if (selectedEvent == EventType::Party) {
                    if (selectedPartyOption == PartyOption::Option1) {
                        newEvent.tasks.assign(partyOption1Tasks.begin(), partyOption1Tasks.end());
                    } else if (selectedPartyOption == PartyOption::Option2) {
                        newEvent.tasks.assign(partyOption2Tasks.begin(), partyOption2Tasks.end());
                    } else if (selectedPartyOption == PartyOption::Option3) {
                        newEvent.tasks.assign(partyOption3Tasks.begin(), partyOption3Tasks.end());
                    } else if (selectedPartyOption == PartyOption::Option4) {
                        newEvent.tasks.assign(partyOption4Tasks.begin(), partyOption4Tasks.end());
                    } else if (selectedPartyOption == PartyOption::Option5) {
                        newEvent.tasks.assign(partyOption5Tasks.begin(), partyOption5Tasks.end());
                    }
                } else if (selectedEvent == EventType::Conference) {
                    if (selectedConferenceOption == ConferenceOption::Option1) {
                        newEvent.tasks.assign(conferenceOption1Tasks.begin(), conferenceOption1Tasks.end());
                    } else if (selectedConferenceOption == ConferenceOption::Option2) {
                        newEvent.tasks.assign(conferenceOption2Tasks.begin(), conferenceOption2Tasks.end());
                    } else if (selectedConferenceOption == ConferenceOption::Option3) {
                        newEvent.tasks.assign(conferenceOption3Tasks.begin(), conferenceOption3Tasks.end());
                    } else if (selectedConferenceOption == ConferenceOption::Option4) {
                        newEvent.tasks.assign(conferenceOption4Tasks.begin(), conferenceOption4Tasks.end());
                    } else if (selectedConferenceOption == ConferenceOption::Option5) {
                        newEvent.tasks.assign(conferenceOption5Tasks.begin(), conferenceOption5Tasks.end());
                    } else if (selectedConferenceOption == ConferenceOption::Option6) {
                        newEvent.tasks.assign(conferenceOption6Tasks.begin(), conferenceOption6Tasks.end());
                    }
                } else if (selectedEvent == EventType::Engagement) {
                    if (selectedEngagementOption == EngagementOption::Option1) {
                        newEvent.tasks.assign(engagementOption1Tasks.begin(), engagementOption1Tasks.end());
                    } else if (selectedEngagementOption == EngagementOption::Option2) {
                        newEvent.tasks.assign(engagementOption2Tasks.begin(), engagementOption2Tasks.end());
                    } else if (selectedEngagementOption == EngagementOption::Option3) {
                        newEvent.tasks.assign(engagementOption3Tasks.begin(), engagementOption3Tasks.end());
                    } else if (selectedEngagementOption == EngagementOption::Option4) {
                        newEvent.tasks.assign(engagementOption4Tasks.begin(), engagementOption4Tasks.end());
                    } else if (selectedEngagementOption == EngagementOption::Option5) {
                        newEvent.tasks.assign(engagementOption5Tasks.begin(), engagementOption5Tasks.end());
                    }
                } else {
                    newEvent.tasks = {"Plan Details", "Book Venue", "Arrange Catering", "Send Invitations", "Finalize Guest List"};
                }
                newEvent.taskCompleted.assign(newEvent.tasks.size(), false);
                
                savedEvents.push_back(newEvent);
                currentEvent = &savedEvents.back();
                saveEvent(newEvent);
                
                // Set dashboard variables
                strcpy(selectedEventName, newEvent.name.c_str());
                strcpy(selectedEventDate, newEvent.date.c_str());
                strcpy(selectedEventLocation, newEvent.location.c_str());
                strcpy(selectedEventGuests, newEvent.guestList.c_str());
                totalBudget = newEvent.budget;
                spentBudget = 0.0f;
                
                // Set up tasks for dashboard
                currentEventTasks = newEvent.tasks;
                UpdateDynamicTasks(currentEventTasks);
                
                // Reset form
                strcpy(eventName, "");
                strcpy(eventDate, "");
                strcpy(eventLocation, "");
                eventBudget = 0.0f;
                strcpy(eventGuests, "");
                strcpy(eventNotes, "");
                
                currentScreen = SCREEN_DASHBOARD;
            }
            
            if (ImGui::Button("Back", ImVec2(450, 40))) {
                currentScreen = SCREEN_EVENT_SELECTION;
            }
            
            ImGui::PopFont();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
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
        else if (currentScreen == SCREEN_CALENDAR)
        {
            RenderCalendarView(calendarState, "Ahmed");
        }
        else if (currentScreen == SCREEN_BUDGET_SYSTEM)
        {
            RenderBudgetSystem();
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
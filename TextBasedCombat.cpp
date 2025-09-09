#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <limits>
#include <cstdlib>
using namespace std;

// Function to set console color (for fun!)
void SetConsoleColor(int text, int background) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, text | (background << 4));
}

// Function for a little pause effect
void Pause() {
  cout << "\nPress ENTER to continue...\n";
  cin.ignore();
  cin.get();
}

// Title screen
void ShowTitleScreen() {
  system("cls"); // clear screen
  SetConsoleColor(10, 0); // Green text
  cout << "=============================================\n";
  cout << "        WELCOME TO MY ADVENTURE GAME!        \n";
  cout << "=============================================\n";
  SetConsoleColor(7, 0); // Reset to default

  cout << "\n1. Start Game\n";
  cout << "2. Instructions\n";
  cout << "3. Exit\n";

  int choice;
  cout << "\nChoose an option: ";
  cin >> choice;

  switch (choice) {
  case 1:
    cout << "\nStarting game...\n";
    Pause();
    break;
  case 2:
    system("cls");
    cout << "Instructions:\n";
    cout << "- Explore the world.\n";
    cout << "- Fight enemies.\n";
    cout << "- Collect loot and upgrade gear.\n";
    cout << "- Survive as long as you can!\n";
    Pause();
    ShowTitleScreen(); // return to menu
    break;
  case 3:
    cout << "\nThanks for playing!\n";
    exit(0);
  default:
    cout << "\nInvalid choice.\n";
    Pause();
    ShowTitleScreen();
  }
}


struct Item {
  string name;
  string slot;
  bool isEquipped = false;
  int attackBonus;
  int defenseBonus;
  int healthBonus;

  Item(string n, string s, int atk, int def, int hp)
    : name(n), slot(s), attackBonus(atk), defenseBonus(def), healthBonus(hp){
  }
};


class Player {
public:
  string name;
  int health = 100;
  vector<Item> inventory;
  int healthPotions = 15;
  int exp = 0;
  int maxHealth = 100;

  Player(string playerName) {
    name = playerName;
    health = 100;
  }

  void TakeDamage(int dmg) {
    health -= dmg;
    if (health < 0) health = 0;
    SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY, 0);
    cout << name << " takes " << dmg << " damage! Health: " << health << "/" << maxHealth << endl;
    SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, 0);
  }

  void AddItem(Item newItem) {
    inventory.push_back(newItem);
    cout << name << " picked up a " << newItem.name << "!" << endl;
  }

  void ShowInventory() {
    cout << "|----------------------------------------|\n";
    cout << "\n   " << name << "'s Inventory: ";
    if (!inventory.empty()) {
      int itemCount = 1;
      for (auto& item : inventory) {
        cout << "\n      " << itemCount++ << ". "
          << item.name
          << " [" << item.slot << "]"
          << " (ATK: " << item.attackBonus
          << ", DEF: " << item.defenseBonus
          << ", HP: " << item.healthBonus << ")";
        if (item.isEquipped) cout << " (Equipped)";
      }
      cout << endl;
    }
    else {
      cout << "empty\n";
    }
    cout << "|----------------------------------------|\n";
  }

  void Equip() {
    cout << "\n--------------------------------\n";
    cout << "  What would you like to equip?\n";
    cout << "--------------------------------\n";

    int index = 1;
    for (auto& item : inventory) {
      if (!item.isEquipped) {
        cout << "   " << index++ << ". " << item.name << endl;
      }
    }
    if (index == 1) {
      cout << "   (No items available to equip)\n";
      return;
    }

    cout << "Enter item name exactly: ";
    cin.ignore();
    string equipInput;
    getline(cin, equipInput);

    bool found = false;
    for (auto& item : inventory) {
      if (item.name == equipInput) {
        if (!item.isEquipped) {
          item.isEquipped = true;
          found = true;
          cout << "You equipped " << item.name << "!\n";
          if (item.name == "The Shard of the First Skirmish") {
            maxHealth = 150;
            health = 150;
          }
          else if (item.name == "The Vengeful Eye") {
            maxHealth = 200;
            health = 200;
          }
        } else {
            cout << item.name << " is already equipped.\n";
          }
      }
    }
    if (!found) cout << "Invalid input.\n";
  }

  void Heal() {
    if (healthPotions > 0) {
      health += 40;
      if (health > maxHealth) {
        health = maxHealth;
      }
      healthPotions--;
      SetConsoleColor(FOREGROUND_GREEN, 0);
      cout << name << " heals! Health: " << health << "/" << maxHealth << endl;
      SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, 0);
      cout << "Health potions remaining: " << healthPotions << "\n";
    }
    else {
      cout << "You don't have any more health potions.\n";
    }
  }

  int GetTotalAttack() {
    int total = 5; // base attack
    for (auto& item : inventory) {
      if (item.isEquipped) total += item.attackBonus;
    }
    return total;
  }

  int GetTotalDefense() {
    int total = 0;
    for (auto& item : inventory) {
      if (item.isEquipped) total += item.defenseBonus;
    }
    return total;
  }

  int GetTotalHealth() {
    int total = 100;
    for (auto& item : inventory) {
      if (item.isEquipped) total += item.healthBonus;
    }
    return total;
  }
};

class Enemy {
public:
  string type;
  int health;
  int attack;

  Enemy(string enemyType, int enemyHealth, int enemyAttack)
    : type(enemyType), health(enemyHealth), attack(enemyAttack) {
  }

  void TakeDamage(int dmg) {
    health -= dmg;
    if (health < 0) health = 0;
    SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN, 0);
    cout << type << " takes " << dmg << " damage! Health: " << health << endl;
    SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, 0);
  }

  bool IsAlive() {
    return health > 0;
  }
};

class Location {
public:
  string name;
  string description;
  vector<Enemy> enemies;
  vector<Item> lootTable;

  Location(string n, string d, vector<Enemy> e, vector<Item> l) {
    name = n;
    description = d;
    enemies = e;
    lootTable = l;
  }
};

void Explore(Player& player, Location& location) {
  cout << "\nYou enter the " << location.name << "...\n";
  cout << location.description << endl;

  int enemyIndex = 0;
  int lootIndex = 0;
  Enemy enemy = location.enemies[enemyIndex];

  SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN, 0);
  cout << "\nA " << enemy.type << " has appeared!\n";
  SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, 0);

  while (player.health > 0) {
    cout << "\nChoose an action (attack(1) / heal(2) / inventory(3) / equip(4) / flee(5): ";
    int action;
    cin >> action;

    if (action == 1) {
      enemy.TakeDamage(player.GetTotalAttack());

      if (enemy.IsAlive()) {
        int damage = enemy.attack - player.GetTotalDefense();
        if (damage < 0) damage = 0;
        player.TakeDamage(damage);
      }
      else {
        SetConsoleColor(FOREGROUND_GREEN, 0);
        cout << player.name << " defeated " << enemy.type << "!\n";
        SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, 0);

        if (!location.lootTable.empty() && lootIndex < location.lootTable.size()) {
          Item loot = location.lootTable[lootIndex++];
          player.AddItem(loot);
          if (lootIndex == 6) {
            return;
          }
        }

        enemyIndex = (enemyIndex + 1) % location.enemies.size();
        enemy = location.enemies[enemyIndex];
        if (enemy.type == "Kharvox, Voice of the Forgotten Age") {
          SetConsoleColor(FOREGROUND_RED, 0);
          cout << "\nA " << enemy.type << " has appeared!\n";
          SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, 0);
        }
        else
        {
          SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN, 0);
          cout << "\nA " << enemy.type << " has appeared!\n";
          SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, 0);
        }
      }
    }
    else if (action == 2) player.Heal();
    else if (action == 3) player.ShowInventory();
    else if (action == 4) player.Equip();
    else if (action == 5 && location.name != "Cave of Flames") {
      cout << player.name << " flees from the " << location.name << " never to return...\n";
      return;
    }
    else if (action == 5 && location.name == "Cave of Flames") {
      player.health = 0;
      cout << player.name << " attempts to flee from the " << location.name << " and gets engulfed in flames...\n";
      return;
    }
    else {
      cout << "Invalid action!\n" << "Please enter the number associated with an action.\n";
      cin.clear();
      cin.ignore(100,'\n');
    }

    if (enemy.type == "Kharvox, Voice of the Forgotten Age" && enemy.health <= 300) {
      cout << enemy.type << " flees from the mighty " << player.name << "...\n";
      return;
    }
  }
}

int main() {
  ShowTitleScreen();
  cout << "Enter your character's name: ";
  string playerName;
  getline(cin, playerName);

  cout << "Welcome to the Fantasy Adventure Demo, " << playerName << "!\nGoal - Survive by equipping items from defeated enemies.\nGood luck!\n\n";

  Player player(playerName);

  vector<Location> world = {
      Location("Dark Forest", "A twisted forest filled with goblins.",
          { Enemy("Ranged Goblin", 30, 10), Enemy("Goblin Brute", 50, 15), Enemy("Goblin Boss", 70, 20)},
          { Item("Goblin Breastplate", "Armor", 0, 5, 0), Item("Goblin Gauntlets", "Armor", 0, 2, 0), Item("Goblin Helmet", "Armor", 0, 3, 0), Item("Goblin Chausses", "Armor", 0, 4, 0), Item("Goblin Sabaton", "Armor", 0, 2, 0), Item("The Shard of the First Skirmish", "Relic", 0, 0, 50)}),

      Location("Lost Ruins of Veldor", "The air suddenly thickens with a potent odor of orcs.",
          { Enemy("Orc Brute", 90, 18), Enemy("Orc Mage", 100, 20), Enemy("Orc Boss", 110, 25)},
          { Item("Rusty Blade", "Weapon", 5, 0, 0), Item("Bronze Sword", "Weapon", 10, 0, 0), Item("Iron Sword", "Weapon", 15, 0, 0), Item("Steel Sword", "Weapon", 20, 0, 0), Item("Obsidian Sword", "Weapon", 30, 0, 0), Item("The Vengeful Eye", "Relic", 0, 0, 50)}),

      Location("Cave of Flames", "The cave continuously burns with black flames and a ferocious roar can be heard from afar.",
          { Enemy("Kharvox, Voice of the Forgotten Age", 500, 65) },
          { Item("Dragon Scale", "Relic", 0, 10, 0) })
  };

  for (Location& loc : world) {
    Explore(player, loc);
    if (player.health <= 0) {
      cout << "Game Over!\n";
      break;
    }
    else if (player.health > 0 && loc.name == "Cave of Flames") {
      cout << "VICTORY!\n";
    }
  }
}

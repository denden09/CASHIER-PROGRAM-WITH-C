// ============= INCLUDES =============
#include <stdio.h>	// Input dan output
#include <malloc.h>	// Alokasi memori
#include <string.h>	// Operasi string
#include <conio.h>	// Untuk getch()
#include <time.h>	// Operasi waktu

// ============= DEFINES =============
#define PROGRAM_NAME					"      INDOAPRIL by Kelompok C\
									   \n=======================================\n\n"

#define ADMINISTRATOR_ID				"admin"				// ID admin utama
#define ADMINISTRATOR_PASSWORD			"admin"				// Password admin utama

// Lokasi file
#define FILE_INFORMATIONS				"informations.txt"	// Menyimpan informasi umum
#define FILE_ACCOUNTS					"accounts.txt"		// Menyimpan data-data akun kasir
#define FILE_ITEMS						"items.txt"			// Menyimpan data-data barang
#define FILE_HISTORY					"history.txt"		// Menyimpan riwayat penjualan
#define FILE_RECEIPT					"receipt.txt"		// Media pengganti kertas untuk print struk belanja

// Informasi tambahan
#define MAX_LOGIN_ATTEMPT				3					// Kesempatan untuk login sebelum dikeluarkan dari program

#define MAX_ITEM_SELL_PER_ACTION		100					// Maksimal tipe barang yang dijual dalam satu waktu

#define MIN_ACCOUNT_ID_LENGTH			5					// Minimal karakter untuk ID kasir
#define MAX_ACCOUNT_ID_LENGTH			50					// Maksimal karakter untuk ID kasir
#define MIN_ACCOUNT_PASSWORD_LENGTH		5					// Minimal karakter untuk password kasir
#define MAX_ACCOUNT_PASSWORD_LENGTH		50					// Maksimal karakter untuk password kasir
#define MIN_ACCOUNT_NAME_LENGTH			5					// Minimal karakter untuk nama kasir
#define MAX_ACCOUNT_NAME_LENGTH			50					// Maksimal karakter untuk nama kasir

#define MAX_ITEM_CODE_LENGTH			11					// Maksimal digit untuk kode barang
#define MAX_ITEM_NAME_LENGTH			50					// Maksimal karakter untuk nama barang

// ============= STRUCTURES =============
// Informasi umum
typedef struct Information {
	int total;
} Information;

Information information = {0};

// Data-data akun
typedef struct Account {
	char id[MAX_ACCOUNT_ID_LENGTH+1];
	char password[MAX_ACCOUNT_PASSWORD_LENGTH+1];
	char name[MAX_ACCOUNT_NAME_LENGTH+1];

	unsigned short employment;

	struct Account *prev, *next;
} Account;

const Account DEFAULT_ACCOUNT = {"", "", "", 0};
Account
	account,
	*account_Head = NULL,
	*account_Tail = NULL;

// Data-data barang
typedef struct Item {
	// Data informasi barang
	long long code;
	char name[MAX_ITEM_NAME_LENGTH+1];
	int price;
	int quantity;

	// Struktur data AVL Binary Tree
	int height;
	struct Item *left, *right, *parent;
} Item;

Item *root = NULL;

// ============= DECLARATIONS =============
void account_Login();

void menu_Main();

void item_Sell();
void item_Add();
void item_Add_New();
void item_Add_Existed();
void item_View_All();
void item_History();
void account_Search();
void account_View_All();
void account_Add();
void account_Delete();

_Bool account_Push(const Account acc);
_Bool account_Pop(Account *node);

void swap(Account *a, Account *b);
Account *partition(Account *l, Account *h);
void account_Sort(Account *l, Account *h);

void item_ReadFile();
void item_WriteFile();

Item *item_Get(Item *node, const long long code);
Account account_Get(const char *id);

char *inputPassword();
long long strToCode(const char *str);

void screen_Clear();
void screen_Pause();

int height(const Item *N);
int max(const int value_1, const int value_2);
Item *newItem(const int item);
Item *leftRotate(Item *x);
Item *rightRotate(Item *y);
Item *minValueItem(Item *node);
int getBalance(const Item *N);
void item_Insert(Item **node, Item newItem);
void item_Delete(Item **node, long long code);

// ============= FUNCTIONS =============
int main() {
	account_Login();

	return 0;
}

void account_Login() {
	account = DEFAULT_ACCOUNT;

	char temp_ID[MAX_ACCOUNT_ID_LENGTH+1];
	char temp_Password[MAX_ACCOUNT_PASSWORD_LENGTH+1];

	int wrong_Password = 0;

	screen_Clear();
	printf("Selamat datang, silakan login untuk melanjutkan\n\n");
	while(1) {
		printf("ID       : "); scanf(" %[^\n]s", temp_ID); fflush(stdin);
		printf("Password : ");
		strcpy(temp_Password, inputPassword());

		if(strcmp(ADMINISTRATOR_ID, temp_ID) == 0 && strcmp(ADMINISTRATOR_PASSWORD, temp_Password) == 0) {
			printf("\n\nBerhasil masuk sebagai admin utama\n\n");
			strcpy(account.id, ADMINISTRATOR_ID);
			strcpy(account.password, ADMINISTRATOR_PASSWORD);
			strcpy(account.name, "Administrator");

			account.employment = 2;

			screen_Pause();
			menu_Main();
			break;
		}
		else {
			account = account_Get(temp_ID);

			if(account.id[0] != '\0' && strcmp(account.password, temp_Password) == 0) {
				if(!account.employment) printf("\n\nBerhasil masuk sebagai kasir, %s\n\n", account.name);
				else printf("\n\nBerhasil masuk sebagai moderator, %s\n\n", account.name);

				screen_Pause();
				menu_Main();
			}
			else {
				wrong_Password++;
				
				if(wrong_Password >= MAX_LOGIN_ATTEMPT) {
					printf("\n\nAnda telah gagal login sebanyak %d kali. Menutup program...\n\n", wrong_Password);
					break;
				}

				screen_Clear();
				printf("ID atau password salah. (%d/%d)\n\n", wrong_Password, MAX_LOGIN_ATTEMPT);
			}
		}
	}
}

void menu_Main() {
	item_ReadFile();

	int choose;

	while(1) {
		screen_Clear();
		printf("==============================\n");

		if(account.employment == 1) printf("  %s, Moderator\n", account.name);
		else if(account.employment == 2) printf("  Administrator\n");
		else printf("  %s, Kasir\n", account.name);

		printf("==============================\n");
		printf("1. Jual barang\n");
		printf("2. Tambah barang\n");
		printf("3. Daftar harga barang\n");
		printf("4. Riwayat penjualan\n\n");

		if(account.employment) {
			printf("5. Cari akun\n");
			printf("6. List akun\n");
			printf("7. Tambah akun\n");
			printf("8. Hapus akun\n\n");
		}

		printf("0. >> LOGOUT <<\n");
		printf("Pilih : "); scanf("%d", &choose); fflush(stdin);
		screen_Clear();

		switch(choose) {
			case 1 :
				item_Sell();
				break;
			case 2 :
				item_Add();
				break;
			case 3 :
				item_View_All();
				screen_Pause();
				break;
			case 4 :
				item_History();
				break;
			case 5 :
				account_Search();
				break;
			case 6 :
				if(!account.employment) return;
				account_Sort(account_Head, account_Tail);
				account_View_All();
				screen_Pause();
				break;
			case 7 :
				if(!account.employment) return;
				account_Add();
				break;
			case 8 :
				if(!account.employment) return;
				account_Delete();
				break;
			default :
				return;
		}
	}
}

void item_Sell() {
	struct Cart {
		long long code;
		char name[MAX_ITEM_NAME_LENGTH+1];
		int price;
		int quantity;
	} cart[MAX_ITEM_SELL_PER_ACTION];

	Item *item;
	char temp_itemCode[MAX_ITEM_CODE_LENGTH+1], more;
	int top = 0, total = 0;

	while(1) {
		item_View_All();

		if(root == NULL) {
			screen_Pause();
			break;
		}
		else {
			printf("\nKode barang : "); scanf(" %[^\n]s", temp_itemCode); fflush(stdin);
			cart[top].code = strToCode(temp_itemCode);
			item = item_Get(root, cart[top].code);

			if(item == NULL)
				printf("\nBarang tidak ditemukan!\n");
			else {
				while(1) {
					printf("Jumlah      : "); scanf("%d", &cart[top].quantity); fflush(stdin);
					if(cart[top].quantity < 1 || cart[top].quantity > item->quantity)
						printf("Angka yang dimasukkan tidak valid\n");
					else {
						strcpy(cart[top].name, item->name);
						cart[top].price = item->price;
						break;
					}
				}

				top++;
			}

			printf("\nJual barang lagi? [y/n] : "); scanf("%c", &more); fflush(stdin);
			if(more == 'y' || more == 'Y') continue;
			else {
				if(top == 0) break;
				else {
					screen_Clear();

					printf("                   KERANJANG");
					printf("\n===============================================\n\n");
					for(int i = 0; i < top; ++i) {
						if(cart[i].quantity > 0) {
							printf("%-20s @ Rp %-6d x %-3d = %d\n",
								cart[i].name, cart[i].price, cart[i].quantity, cart[i].price * cart[i].quantity);
							total += (cart[i].price * cart[i].quantity);
						}
					}
					printf("\n===============================================\n");
					printf("                              Total : Rp %d", total);
					printf("\n===============================================\n");

					FILE *fp = fopen(FILE_RECEIPT, "w");
					fprintf(fp, "                  STRUK BELANJA");
					fprintf(fp, "\n===============================================\n\n");
					for(int i = 0; i < top; ++i)
						if(cart[i].quantity > 0)
							fprintf(fp, "%-20s @ Rp %-6d x %-3d = %d\n",
								cart[i].name, cart[i].price, cart[i].quantity, cart[i].price * cart[i].quantity);
					fprintf(fp, "\n===============================================\n");
					fprintf(fp, "                              Total : Rp %d", total);
					fprintf(fp, "\n===============================================\n");
					fclose(fp);

					information.total += total;

					fp = fopen(FILE_INFORMATIONS, "w");
					fprintf(fp, "Total Terjual : %d\n", information.total);
					fclose(fp);

					fp = fopen(FILE_HISTORY, "a");

					time_t my_time;
					struct tm * timeinfo;
					time(&my_time);
					timeinfo = localtime(&my_time);

					for(int i = 0; i < top; ++i) {
						fprintf(fp, "%-20s @ Rp %-7d x %-3d = Rp %-9d - %.2d/%.2d/%.4d %.2d:%.2d   - %s\n",
								cart[i].name, cart[i].price, cart[i].quantity, cart[i].price * cart[i].quantity,
								timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min,
								account.name);

						item = item_Get(root, cart[i].code);
						item->quantity -= cart[i].quantity;
						if(item->quantity <= 0)
							item_Delete(&root, cart[i].code);
					}
					fclose(fp);

					item_WriteFile();

					printf("\nBerhasil menjual barang\n\n");
					screen_Pause();

					break;
				}
			}
		}
	}
}

void item_Add() {
	int choose;

	while(1) {
		screen_Clear();
		printf("1. Tambah jumlah barang yang sudah ada\n");
		printf("2. Buat barang baru\n\n");
		printf("0. >> KEMBALI <<\n");
		printf("Pilih : "); scanf("%d", &choose); fflush(stdin);

		switch(choose) {
			case 1 :
				item_Add_Existed();
				break;
			case 2 :
				item_Add_New();
				break;
			default :
				return;
		}
	}
}

void item_Add_New() {
	screen_Clear();

	Item *node = (Item *) malloc(sizeof(Item));

	if(node != NULL) {
		printf("Nama   : "); scanf(" %[^\n]s", node->name); fflush(stdin);
		printf("Harga  : Rp "); scanf("%d", &node->price); fflush(stdin);
		printf("Jumlah : "); scanf("%d", &node->quantity); fflush(stdin);

		node->code = 1;
		node->left = node->right = node->parent = NULL;
		node->height = 1;

		if(root == NULL)
			root = node;
		else {
			Item *curr = root;
			_Bool leftdone = 0;

			while(curr) {
				if(leftdone == 0) {
					while(curr->left != NULL)
						curr = curr->left;
				}

				if(node->code < curr->code) {
					item_Insert(&root, *node);
					break;
				}
				(node->code)++;

				leftdone = 1;

				if(curr->right != NULL) {
					leftdone = 0;
					curr = curr->right;
				}
				else if(curr->parent != NULL) {
					while(curr->parent != NULL && curr == curr->parent->right)
						curr = curr->parent;
					if(curr->parent == NULL)
						break;
					curr = curr->parent;
				}
				else
					break;
			}
		}

		item_WriteFile();

		printf("\nBerhasil menambahkan barang BARU (%.11lld) %s, sebanyak %d\n\n", node->code, node->name, node->quantity);
		screen_Pause();
	}
}

void item_Add_Existed() {
	item_View_All();

	char temp_itemCode[MAX_ITEM_CODE_LENGTH+1];
	int temp_itemQuantity;

	if(root == NULL) {
		screen_Pause();
		return;
	}

	printf("Kode barang : "); scanf(" %[^\n]s", temp_itemCode); fflush(stdin);
	Item *item = item_Get(root, strToCode(temp_itemCode));
	
	if(item == NULL)
		printf("\nBarang tidak ditemukan!\n");
	else {
		printf("\n[ Untuk mengurangi jumlah barang, gunakan tanda \"-\" ]\n");
		printf("Jumlah      : "); scanf("%d", &temp_itemQuantity); fflush(stdin);

		printf("\nBerhasil menambahkan barang (%.11lld) %s, sebanyak %d\n\n", item->code, item->name, temp_itemQuantity);
		
		item->quantity += temp_itemQuantity;

		if(item->quantity <= 0)
			item_Delete(&root, item->code);

		item_WriteFile();
	}
	screen_Pause();
}

void item_View_All() {
	screen_Clear();
	printf("                        Daftar harga barang\n");
	printf("-----------------------------------------------------------------------\n");
	printf("|    Kode     |              Nama              |    Harga    | Jumlah |\n");
	printf("|---------------------------------------------------------------------|\n");

	if(root == NULL)
		printf("|     Tidak ada barang                                            |\n");
	else {
		Item *curr = root;
		_Bool leftdone = 0;

		while(curr) {
			if(leftdone == 0) {
				while(curr->left != NULL)
					curr = curr->left;
			}

			printf("| %.11lld | %-30s | Rp %8d | %d |\n", curr->code, curr->name, curr->price, curr->quantity);
			leftdone = 1;

			if(curr->right != NULL) {
				leftdone = 0;
				curr = curr->right;
			}
			else if(curr->parent != NULL) {
				while(curr->parent != NULL && curr == curr->parent->right)
					curr = curr->parent;
				if(curr->parent == NULL)
					break;
				curr = curr->parent;
			}
			else
				break;
		}
	}

	printf("-----------------------------------------------------------------------\n\n");
}

void item_History() {
	screen_Clear();

	FILE *fp = fopen(FILE_HISTORY, "r");
	if(fp != NULL) {
		fseek(fp, 0, SEEK_END);
		if(ftell(fp)) {
			fseek(fp, 0, SEEK_SET);

			char str[128];

			while(!feof(fp)) {
				fscanf(fp, "%[^\n]\n", str);
				printf("%s\n", str);
			}
		}
		else printf("Tidak ada riwayat apapun\n");
	}
	else printf("File tidak ditemukan\n");
	fclose(fp);

	printf("\n");

	if(account.employment) {
		int choose;

		printf("1. Hapus semua riwayat\n\n");
		printf("0. >> KEMBALI <<\n");
		printf("Pilih : "); scanf("%d", &choose); fflush(stdin);

		switch(choose) {
			case 1 :
				fclose(fopen(FILE_HISTORY, "w"));
				printf("Semua riwayat penjualan berhasil dihapus\n\n");
				break;

			default :
				return;
		}
	}

	screen_Pause();
}

void account_Search() {
	char id[MAX_ACCOUNT_ID_LENGTH+1];
	printf("Masukkan ID : "); scanf(" %[^\n]s", id); fflush(stdin);

	Account *curr = account_Head;

	while(curr != NULL && strcmp(curr->id, id) != 0)
		curr = curr->next;
	
	if(curr != NULL) {
		printf("\nNama   : %s\n", curr->name);
		printf("ID     : %s\n", curr->id);
		printf("Posisi : %s\n\n", curr->employment ? "Moderator" : "Kasir");
	}
	else printf("\nID tidak ditemukan!\n\n");

	screen_Pause();
}

void account_View_All() {
	printf("                      List Akun Kasir\n");
	printf("------------------------------------------------------------\n");
	printf("|No.|         Nama         |        ID        |   Posisi   |\n");
	printf("|----------------------------------------------------------|\n");

	if(account_Head == NULL)
		printf("|     Belum ada akun kasir                                 |\n");
	else {
		int i = 1;
		for(Account *curr = account_Head; curr != NULL; curr = curr->next, ++i)
			printf("|%-3d| %-20s | %-16s | %-10s |\n", i, curr->name, curr->id, !curr->employment ? "Kasir" : "Moderator");
	}

	printf("------------------------------------------------------------\n\n");
}

void account_Add() {
	Account acc = DEFAULT_ACCOUNT;

	int choose;

	while(1) {
		screen_Clear();

		if(acc.id[0] == '\0') printf("1. ID\n");
		else printf("1. ID        : %s\n", acc.id);

		if(acc.password[0] == '\0') printf("2. Password\n\n");
		else printf("2. Password  : %s\n\n", acc.password);

		if(acc.name[0] == '\0') printf("3. Nama\n\n");
		else printf("3. Nama      : %s\n\n", acc.name);

		if(!acc.employment) printf("4. Moderator : Tidak\n\n");
		else printf("4. Moderator : Ya\n\n");

		printf("9. >> KONFIRMASI <<\n");
		printf("0. >>  KEMBALI   <<\n");
		printf("Pilih : "); scanf("%d", &choose); fflush(stdin);
		screen_Clear();

		switch(choose) {
			case 1 :
				while(1) {
					printf("Masukkan ID : "); scanf(" %[^\n]s", acc.id); fflush(stdin);
					if(strlen(acc.id) < MIN_ACCOUNT_ID_LENGTH || strlen(acc.id) > MAX_ACCOUNT_ID_LENGTH)
						printf("ID harus terdiri dari %d - %d karakter\n\n", MIN_ACCOUNT_ID_LENGTH, MAX_ACCOUNT_ID_LENGTH);
					else break;
				}
				break;
			case 2 :
				while(1) {
					printf("Masukkan password : "); scanf(" %[^\n]s", acc.password); fflush(stdin);
					if(strlen(acc.password) < MIN_ACCOUNT_PASSWORD_LENGTH || strlen(acc.password) > MAX_ACCOUNT_PASSWORD_LENGTH)
						printf("Password harus terdiri dari %d - %d karakter\n\n", MIN_ACCOUNT_PASSWORD_LENGTH, MAX_ACCOUNT_PASSWORD_LENGTH);
					else break;
				}
				break;
			case 3 :
				while(1) {
					printf("Masukkan nama : "); scanf(" %[^\n]s", acc.name); fflush(stdin);
					if(strlen(acc.name) < MIN_ACCOUNT_NAME_LENGTH || strlen(acc.name) > MAX_ACCOUNT_NAME_LENGTH)
						printf("Nama harus terdiri dari %d - %d karakter\n\n", MIN_ACCOUNT_NAME_LENGTH, MAX_ACCOUNT_NAME_LENGTH);
					else break;
				}
				break;
			case 4 :
				acc.employment = !(acc.employment) ? 1 : 0;
				break;
			case 9 : {
				_Bool
					error_ID = (acc.id[0] == '\0'),
					error_Password = (acc.password[0] == '\0'),
					error_Name = (acc.name[0] == '\0');
				
				if(error_ID || error_Password || error_Name) {
					printf("\nEROR :\n");
					if(error_ID) printf("* Tolong masukkan ID\n");
					if(error_Password) printf("* Tolong masukkan password\n");
					if(error_Name) printf("* Tolong masukkan nama\n");

					screen_Pause();
				}
				else {
					if(!account_Push(acc))
						printf("\nNama atau ID telah digunakan!\n\n");
					else {
						FILE *fp = fopen(FILE_ACCOUNTS, "a");
							fprintf(fp, "%s#%s#%s#%d\n", acc.name, acc.id, acc.password, acc.employment);
						fclose(fp);

						printf("\nBerhasil menambahkan akun kasir\n\n");
						screen_Pause();

						return;
					}
				}

				break;
			}
			default :
				return;
		}
	}
}

void account_Delete() {
	if(account_Head == NULL)
		printf("Belum ada akun kasir\n\n");
	else {
		account_View_All();

		int key;
		printf("Masukkan nomor : "); scanf("%d", &key); fflush(stdin);

		Account *curr = account_Head;
		for(int i = 1; i < key && curr != NULL; ++i)
			curr = curr->next;
		
		if(key > 0 && curr != NULL) {
			if(strcmp(curr->id, account.id) != 0) {
				char sure;

				printf("\n  Informasi akun kasir\n");
				printf("------------------------\n");
				printf("Nama        : %s\n", curr->name);
				printf("ID          : %s\n", curr->id);
				printf("Moderator   : %s\n\n", curr->employment ? "Ya" : "Tidak");
				printf("Anda yakin ingin menghapus akun kasir ini? [y/n] : "); scanf("%c", &sure); fflush(stdin);

				if(sure == 'y' || sure == 'Y') {
					char temp_Name[MAX_ACCOUNT_NAME_LENGTH+1];
					strcpy(temp_Name, curr->name);

					account_Pop(curr);

					FILE *fp = fopen(FILE_ACCOUNTS, "w");
					for(curr = account_Head; curr != NULL; curr = curr->next)
						fprintf(fp, "%s#%s#%s#%d\n", curr->name, curr->id, curr->password, curr->employment);
					fclose(fp);

					printf("\nBerhasil menghapus akun kasir %s\n\n", temp_Name);
				}
				else printf("\nBatal menghapus akun\n\n");
			}
			else printf("\nTidak dapat menghapus akun anda sendiri\n\n");
		}
		else printf("\nNomor kasir tidak valid\n\n");
	}

	screen_Pause();
}

_Bool account_Push(const Account acc) {
	Account *node = (Account *) malloc(sizeof(Account));

	if(node != NULL) {
		*node = acc;
		node->prev = node->next = NULL;

		if(account_Head == NULL)
			account_Head = account_Tail = node;
		else {
			account_Tail->next = node;
			node->prev = account_Tail;
			account_Tail = node;
		}
	}
}

/* _Bool account_Push(const Account acc) {
	Account *node = (Account *) malloc(sizeof(Account));

	if(node != NULL) {
		*node = acc;
		node->prev = node->next = NULL;

		if(account_Head == NULL)
			account_Head = account_Tail = node;
		else if(strcmpi(node->name, account_Head->name) < 0) {
			node->next = account_Head;
			account_Head->prev = node;
			account_Head = node;
		}
		else if(strcmpi(node->name, account_Tail->name) > 0) {
			node->prev = account_Tail;
			account_Tail->next = node;
			account_Tail = node;
		}
		else {
			int compResult;
			for(Account *curr = account_Head; curr != NULL; curr = curr->next) {
				if((compResult = strcmpi(node->name, curr->name)) == 0 || strcmp(node->id, curr->id) == 0) {
					free(node);
					return 0;
				}
				else if(compResult <= 0) {
					node->next = curr;
					node->prev = curr->prev;
					curr->prev->next = node;
					curr->prev = node;
					break;
				}
				else continue;
			}
		}
	}

	return 1;
} */

_Bool account_Pop(Account *node) {
	if(node == NULL || account_Head == NULL)
		return 0;

	if(node == account_Head) {
		account_Head = account_Head->next;
		account_Head->prev = NULL;
	}
	else if(node == account_Tail) {
		account_Tail = account_Tail->prev;
		account_Tail->next = NULL;
	}
	else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	free(node);

	return 1;
}

void swap(Account *a, Account *b) {
	Account temp;
	temp = *a;
	*a = *b;
	*b = temp;

	Account *_temp = a->prev;
	a->prev = b->prev;
	b->prev = _temp;

	_temp = a->next;
	a->next = b->next;
	b->next = _temp;
}

Account *partition(Account *l, Account *h) {
	char id[64];
	strcpy(id, h->id);

	Account *i = l->prev;

	for(Account *j = l; j != h; j = j->next) {
		if(strcmp(j->id, id) <= 0) {
			i = (i == NULL) ? l : i->next;
			swap(i, j);
		}
	}

	i = (i == NULL) ? l : i->next;
	swap(i, h);

	return i;
}

void account_Sort(Account *l, Account *h) {
	if(h != NULL && l != h && l != h->next) {
		Account *p = partition(l, h);
		account_Sort(l, p->prev);
		account_Sort(p->next, h);
	}
}

void item_ReadFile() {
	FILE *fp = fopen(FILE_ITEMS, "r");
	if(fp != NULL) {
		fseek(fp, 0, SEEK_END);
		if(ftell(fp)) {
			fseek(fp, 0, SEEK_SET);

			Item item;

			while(!feof(fp)) {
				fscanf(fp, "%lld#%[^#]#%d#%d\n", &item.code, item.name, &item.price, &item.quantity);
				item_Insert(&root, item);
			}
			
			fseek(fp, 0, SEEK_SET);
		}
	}
	fclose(fp);

	fp = fopen(FILE_INFORMATIONS, "r");
	if(fp != NULL) {
		fseek(fp, 0, SEEK_END);
		if(ftell(fp)) {
			fseek(fp, 0, SEEK_SET);
			
			fscanf(fp, "Total Terjual : %d\n", &information.total);
		}
	}
	fclose(fp);

	if(account.employment) {
		fp = fopen(FILE_ACCOUNTS, "r");
		
		if(fp != NULL) {
			fseek(fp, 0, SEEK_END);
			if(ftell(fp)) {
				fseek(fp, 0, SEEK_SET);

				Account acc;

				while(!feof(fp)) {
					fscanf(fp, "%[^#]#%[^#]#%[^#]#%d\n", acc.name, acc.id, acc.password, &acc.employment);
					account_Push(acc);
				}
			}
		}

		fclose(fp);
	}
}

void item_WriteFile() {
	FILE *fp = fopen(FILE_ITEMS, "w");
	
	Item *curr = root;
	_Bool leftdone = 0;

	while(curr) {
		if(leftdone == 0) {
			while(curr->left != NULL)
				curr = curr->left;
		}

		fprintf(fp, "%.11lld#%s#%d#%d\n", curr->code, curr->name, curr->price, curr->quantity);
		leftdone = 1;

		if(curr->right != NULL) {
			leftdone = 0;
			curr = curr->right;
		}
		else if(curr->parent != NULL) {
			while(curr->parent != NULL && curr == curr->parent->right)
				curr = curr->parent;
			if(curr->parent == NULL)
				break;
			curr = curr->parent;
		}
		else
			break;
	}

	fclose(fp);

	fp = fopen(FILE_INFORMATIONS, "w");
	fprintf(fp, "Total Terjual : %d\n", information.total);
	fclose(fp);
}

Item *item_Get(Item *node, const long long code) {
	if(node == NULL || node->code == code)
		return node;
	
	if(node->code < code)
		return item_Get(node->right, code);
	
	return item_Get(node->left, code);
}

Account account_Get(const char *id) {
	FILE *fp = fopen(FILE_ACCOUNTS, "r");

	if(fp != NULL) {
		fseek(fp, 0, SEEK_END);
		if(ftell(fp)) {
			fseek(fp, 0, SEEK_SET);

			Account acc;

			while(!feof(fp)) {
				fscanf(fp, "%[^#]#%[^#]#%[^#]#%d\n", acc.name, acc.id, acc.password, &acc.employment);
				if(strcmp(acc.id, id) == 0) {
					fclose(fp);
					return acc;
				}
			}
		}
	}

	fclose(fp);
	return DEFAULT_ACCOUNT;
}

// Untuk menyamarkan huruf password saat login
char *inputPassword() {
	char
		*password,
		ch;
	int i = 0;

	while(1) {
		ch = getch();

		if(ch == 13 || ch == 9) {
			password[i] = '\0';
			break;
		}
		else if(ch == 8) {
			if(i > 0) {
				i--;
				printf("\b \b");
			}
		}
		else {
			password[i++] = ch;
			printf("* \b");
		}
	}
	return password;
}

// Mengubah tipe data string ke long long integer
// Tidak menggunakan fungsi atoll()
// Karena fungsi dibawah ini langsung mengecek apabila
// digit nilainya melebihi MAX_ITEM_CODE_LENGTH atau tidak
long long strToCode(const char *str) {
    long long var = 0;
    for(int i = 0, j = 1, k = strlen(str); i < k; ++i) {
    	if(j > MAX_ITEM_CODE_LENGTH) return -1;
        if(str[i] < '0' || str[i] > '9') continue;
        var = var*10 + str[i]-'0';
        j++;
    }
    return var;
}

// Sebagai pengganti system("cls");
void screen_Clear() {
	printf("\n\n\n\n\n\n\n\n\n\n");
}

// Sebagai pengganti system("pause");
void screen_Pause() {
	printf("Tekan tombol apapun untuk melanjutkan...\n");
	getch();
}

// ============= AVL BINARY TREE =============
int height(const Item *N) {
	return N == NULL ? 0 : N->height;
}

int max(const int value_1, const int value_2) {
	return (value_1 > value_2) ? value_1 : value_2;
}

Item *leftRotate(Item *x) {
	Item *y = x->right;
	Item *T2 = y->left;

	y->left = x;
	x ->right = T2;

	x->height = max(height(x->left), height(x->right)) + 1;
	y->height = max(height(y->left), height(y->right)) + 1;

	return y;
}

Item *rightRotate(Item *y) {
	Item *x = y->left;
	Item *T2 = x->right;

	x->right = y;
	y->left = T2;

	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;

	return x;
}

int getBalance(const Item *N) {
	return N == NULL ? 0 : height(N->left) - height(N->right);
}

void item_Insert(Item **node, Item newItem) {
	if(*node == NULL) {
		*node = (Item *) malloc(sizeof(Item));

		if(*node != NULL) {
			**node = newItem;
			(*node)->left = (*node)->right = (*node)->parent = NULL;
			(*node)->height = 1;
		}
	}
	else {
		if(newItem.code < (*node)->code) {
			item_Insert(&(*node)->left, newItem);
			(*node)->left->parent = *node;
		}
		else if (newItem.code > (*node)->code) {
			item_Insert(&(*node)->right, newItem);
			(*node)->right->parent = *node;
		}
		else {
			(*node)->height = 1 + max(height((*node)->left), height((*node)->right));

			int balance = getBalance(*node);

			if(balance > 1 && newItem.code < (*node)->left->code)
				*node = rightRotate(*node);

			else if(balance < -1 && newItem.code > (*node)->right->code)
				*node = leftRotate(*node);

			else if(balance > 1 && newItem.code > (*node)->left->code) {
				(*node)->left = leftRotate((*node)->left);
				*node = rightRotate(*node);
			}

			else if(balance < -1 && newItem.code < (*node)->right->code) {
				(*node)->right = rightRotate((*node)->right);
				*node = leftRotate(*node);
			}

			else
				return;
		}
	}
}

Item *minValueItem(Item *node) {
	Item *curr = node;

	while(curr && curr->left != NULL)
		curr = curr->left;

	return curr;
}

void item_Delete(Item **node, long long code) {
	if(*node == NULL)
		return;

	if(code < (*node)->code)
		item_Delete(&(*node)->left, code);
	else if (code > (*node)-> code)
		item_Delete(&(*node)->right, code);
	else {
		if((*node)->left == NULL) {
			Item *temp = (*node)->right;
			free(*node);
			*node = temp;
		}

		else if((*node)->right == NULL) {
			Item *temp = (*node)->left;
			free(*node);
			*node = temp;
		}

		Item *temp = minValueItem((*node)->right);
		(*node)->code = temp->code;
		item_Delete(&(*node)->right, temp->code);
	}
}
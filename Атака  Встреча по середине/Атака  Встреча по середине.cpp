// Атака  Встреча по середине.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream>

using namespace std;
// Реализация S-DES
class SDES
{
private:
	int round1_key[8], round2_key[8];
public:
	void p10(int key[]);
	void p8(int key[]);
	void p4(int s0s1[]);
	void left_shift(int left_half[], int right_half[], int shift_count);	// сдвиг влево на количество раз shift_count
	int* generate_key(int key[], int round);
	void initial_permutation(int pt[]);
	void inverse_initial_permutation(int pt[]);
	int* expand_and_permute(int right_half[]);
	int SBOX0(int row, int column);
	int SBOX1(int row, int column);
	int* rounds(int pt[], int key[], int round_no, int flag);
	int* encode(int pt[], int* round_text, int key[]);
	int* decode(int pt[], int* cipher_text, int key[]);
	int meetInTheMiddle(int* ciphertext, int* plaintext);
};

void SDES::p10(int key[])
{
	//Input:  1 2 3 4 5 6   7 8 9 10
	//Output: 3 5 2 7 4 10 1 9 8 6  
	int out[10] = { 3, 5, 2, 7, 4, 10, 1, 9, 8, 6 };
	int temp[10];
	for (int i = 0; i < 10; i++)		// резервный ключ
		temp[i] = key[i];
	for (int i = 0; i < 10; i++)
	{
		key[i] = temp[out[i] - 1];

	}
}
void SDES::p8(int key[])
{
	//Input:  1 2 3 4 5 6 7  8 9 10
	//Output: 6 3 7 4 8 5 10 9
	int out[8] = { 6, 3, 7, 4, 8, 5, 10, 9 };
	int temp[10];

	for (int i = 0; i < 10; i++)		// резервный ключ
		temp[i] = key[i];
	for (int i = 0; i < 8; i++)
	{
		key[i] = temp[out[i] - 1];

	}
}
void SDES::p4(int s0s1[])  //Прямой P-блок (P4)  2 4 3 1


{
	//Input:  1 2 3 4 
	//Output: 2 4 3 1
	int out[4] = { 2, 4, 3, 1 };
	int temp[4];

	for (int i = 0; i < 4; i++)		// массив резервных копий
		temp[i] = s0s1[i];

	for (int i = 0; i < 4; i++)
	{
		s0s1[i] = temp[out[i] - 1];
	}
}
void SDES::left_shift(int left_half[], int right_half[], int shift_count)
{
	int temp1 = left_half[0];
	int temp2 = right_half[0];

	for (int i = 0; i < 4; i++)
	{
		left_half[i] = left_half[i + 1];
		right_half[i] = right_half[i + 1];
	}
	left_half[4] = temp1;
	right_half[4] = temp2;

	if (shift_count == 2)
		left_shift(left_half, right_half, 1);
}

int* SDES::generate_key(int key[], int round)
{
	int left_half[5], right_half[5];
	static int key1[10], key2[8];
	p10(key);
	for (int i = 0; i < 10; i++)
	{
		if (i < 5)
		{
			left_half[i] = key[i];

		}
		else
		{

			right_half[i - 5] = key[i];

		}
	}

	left_shift(left_half, right_half, 1);

	for (int i = 0; i < 5; i++)		// объедините левую_половину и правую_половину, чтобы сформировать ключ 1
	{
		key1[i] = left_half[i];
		key1[i + 5] = right_half[i];
	}
	if (round == 1)
	{
		p8(key1);
		return key1;
	}
	else
	{
		left_shift(left_half, right_half, 2);
		for (int i = 0; i < 5; i++)		// объедините левую_половину и правую_половину, чтобы сформировать ключ 2
		{
			key2[i] = left_half[i];
			key2[i + 5] = right_half[i];
		}
		p8(key2);
		return key2;
	}
	return nullptr;
}

void SDES::initial_permutation(int pt[])   // Начальная перестановка IP
{
	//Input:  1 2 3 4 5 6 7 8 
   //Output: 2 6 3 1 4 8 5 7 
	int out[8] = { 2,6,3,1,4,8,5,7 };
	int temp[8];

	for (int i = 0; i < 8; i++)		// резервное копирование массива обычного текста
		temp[i] = pt[i];

	for (int i = 0; i < 8; i++)
	{
		pt[i] = temp[out[i] - 1];

	}
}

void SDES::inverse_initial_permutation(int pt[])
{
	//Input:  2 6 3 1 4 8 5 7  
	//Output: 1 2 3 4 5 6 7 8 
	int out[8] = { 2,6,3,1,4,8,5,7 };
	int temp[8];

	for (int i = 0; i < 8; i++)		// резервное копирование массива обычного текста
		temp[i] = pt[i];

	for (int i = 0; i < 8; i++)
	{
		pt[out[i] - 1] = temp[i];
	}
}

int* SDES::expand_and_permute(int right_half[])  //Перестановка с расширением E/P
{
	//Input:  1 2 3 4  
   //Output: 4 1 2 3 2 3 4 1 
	int out[8] = { 4,1,2,3,2,3,4,1 };
	int temp[4];
	static int expanded_right[8];

	for (int i = 0; i < 4; i++)		// резервное копирование массива обычного текста
		temp[i] = right_half[i];

	for (int i = 0; i < 8; i++)
	{
		expanded_right[i] = temp[out[i] - 1];
	}
	return expanded_right;

}

int SDES::SBOX0(int row, int column)
{
	int s0[4][4] = {
			{01,00,11,10},
			{11,10,01,00},
			{00,10,01,11},
			{11,01,11,10}
	};
	return s0[row][column];
}
int SDES::SBOX1(int row, int column)
{
	int s1[4][4] = {
			{00,01,10,11},
			{10,00,01,11},
			{11,00,01,00},
			{10,01,00,11}
	};
	return s1[row][column];
}


int* SDES::rounds(int pt[], int key[], int round_no, int flag)
{
	int left[4], right[4], * expanded_right, s0[4], s1[4], temp_key[10];
	cout << "\nROUND-" << round_no;
	for (int i = 0; i < 10; i++)
	{

		temp_key[i] = key[i];		// резервное копирование исходного ключа по мере его дальнейшего изменения
	}

	if (round_no == 1)
		initial_permutation(pt);	//шаг1 начальная перестановка обычного текста
	for (int i = 0; i < 4; i++)
	{
		left[i] = pt[i];
		right[i] = pt[i + 4];

	}
	expanded_right = expand_and_permute(right);

	static int* key1;
	if (flag == 0)		//флаг=0 для кодирования
	{
		key1 = generate_key(key, round_no);		//ключ1 для раунда1 и ключ2 для раунда2
		if (round_no == 1)
		{
			for (int i = 0; i < 8; i++)
				round1_key[i] = key1[i];		// резервный ключ для декодирования
		}
		else
		{
			for (int i = 0; i < 8; i++)
				round2_key[i] = key1[i];
		}
		cout << "\n\nEncode Key of Round " << round_no << endl;
		for (int i = 0; i < 8; i++)
		{
			cout << key1[i];
		}
	}
	else
	{

		if (round_no == 1)			//если раунд1, используйте ключ2
		{

			for (int i = 0; i < 8; i++)
			{
				key1[i] = round2_key[i];

			}
		}
		else				//если раунд2 использует ключ1
		{

			for (int i = 0; i < 8; i++)
			{

				key1[i] = round1_key[i];
			}
		}

		cout << "\n\nDecode Key of Round " << round_no << endl;
		for (int i = 0; i < 8; i++)
		{
			cout << key1[i];
		}
	}


	for (int i = 0; i < 8; i++)
	{
		expanded_right[i] = expanded_right[i] ^ key1[i];
		if (i < 4)
			s0[i] = expanded_right[i];
		else
			s1[i - 4] = expanded_right[i];
	}

	int row = s0[3] + (s0[0] * 2);			//шаг 4
	int column = s0[2] + (s0[1] * 2);
	static int s0s1[4];
	int ss0 = SBOX0(row, column);

	row = s1[3] + (s1[0] * 2);
	column = s1[2] + (s1[1] * 2);

	int ss1 = SBOX1(row, column);

	s0s1[1] = ss0 % 10;
	s0s1[0] = ss0 / 10;
	s0s1[3] = ss1 % 10;
	s0s1[2] = ss1 / 10;

	p4(s0s1);

	static int new_plain_text[8];

	for (int i = 0; i < 4; i++)
	{
		s0s1[i] = s0s1[i] ^ left[i];
		//поменяйте местами s0s1 и правую половину из шага 1, чтобы сгенерировать обычный текст для следующего раунда
		if (round_no != 2)		//если раунд не 2-й и не предназначен для декодирования
		{
			new_plain_text[i] = right[i];
			new_plain_text[i + 4] = s0s1[i];
		}
		else							//в противном случае не меняйте
		{
			new_plain_text[i + 4] = right[i];
			new_plain_text[i] = s0s1[i];
		}
	}


	cout << "\n\nRound " << round_no << " Output:\n";
	for (int i = 0; i < 8; i++)
		cout << new_plain_text[i] << "\t";
	cout << endl;



	if (round_no == 1)
	{

		if (flag == 0)	//если кодировка
			rounds(new_plain_text, temp_key, 2, 0);
		else			//в противном случае декодирование
			rounds(new_plain_text, temp_key, 2, 1);
	}
	else
	{
		return new_plain_text;
	}

	return nullptr;
}

int* SDES::encode(int pt[], int* round_text, int key[])
{
	round_text = rounds(pt, key, 1, 0);
	inverse_initial_permutation(round_text);

	cout << "\n\n-------------FINAL CIPHER TEXT-------------\n";
	for (int i = 0; i < 8; i++)
		cout << round_text[i];

	return round_text;
	return nullptr;
}

int* SDES::decode(int pt[], int* cipher_text,int key[])
{
	int* new_ct = rounds(cipher_text, key, 1, 1);		//флаг=1 для декодирования
	inverse_initial_permutation(new_ct);

	cout << "\n\n-------------DECODED TEXT-------------\n";
	for (int i = 0; i < 8; i++)
		cout << new_ct[i];
	return new_ct;
}
//// Функция для атаки встречи по середине
int SDES::meetInTheMiddle(int* ciphertext, int* plaintext)
{
   //Перебираем все возможные значения ключей
	for (int i = 0; i <= 0xFF; i++)
	{
		return 0;
		round1_key[i]; // Перебираемый ключ 1
		// Зашифровываем открытый текст с использованием ключа 1
		int* intermediate = encode(round1_key, plaintext,ciphertext);

		// Расшифровываем шифротекст с использованием всех возможных ключей 2
		for (int j = 0; j <= 0xFF; j++) {
			round2_key[j]; // Перебираемый ключ 2

			// Проверяем, соответствует ли расшифрованный текст шифротексту
			if (decode(round2_key, intermediate,plaintext) == ciphertext)
			{
				// Найдено совпадение, возвращаем найденный ключ
				return i;
			}
		}
	}
	// Если ключ не найден
	return 0;
}

int main() 
{
    SDES S;
	int round1_key[0b10101010]; // Известный ключ 1
	int round2_key[0b01010101]; // Известный ключ 2
	int plaintext[0b11001100]; // Исходный текст
	int pt[8];
	// Шифруем текст с использованием известных ключей
	int* intermediate = S.encode(pt, plaintext,round1_key);
	int* ciphertext = S.encode(pt,round2_key, intermediate);

	// Выполняем атаку встречу по середине
	int recoveredKey = S.meetInTheMiddle(ciphertext, plaintext);

	return 0;
}


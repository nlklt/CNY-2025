#include "polis.h"
#include <stack>
#include <queue>

using namespace std;

namespace Polis
{
	/**
	 * @brief Преобразует инфиксное выражение в обратную польскую запись (ОПЗ) в таблице лексем.
	 *
	 * @param lextable_pos Начальная позиция в lextable, с которой начинается выражение (обычно после знака присваивания).
	 * @param lextable Таблица лексем, содержащая входное инфиксное выражение.
	 * @param idtable Таблица идентификаторов (необходима для определения типа идентификатора, например, функция).
	 * @return true, если преобразование выполнено успешно.
	 */
	bool polishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable)
	{
		stack<LT::Entry> stk;		// Создаем **стек для хранения операторов** и скобок (выходной стек алгоритма сортировочной станции)
		queue<LT::Entry> result;	// Создаем **очередь для хранения результата** - лексем в ОПЗ (выходная очередь)
		bool function = false;		// Флаг, указывающий, обрабатывается ли сейчас вызов функции
		int quantityParm = 0;		// Счетчик параметров текущей функции
		int i = ++lextable_pos;		// Начинаем с позиции, следующей за началом выражения (например, после '=')

		// Основной цикл: обрабатываем лексемы до точки с запятой или до правой скобки, если стек не пуст
		for (; lextable.table[i].lexema != LT_SEMICOLON && (lextable.table[i].lexema != LT_RIGHTHESIS || !stk.empty()); i++)
		{
			// Используем switch по лексеме для определения типа токена
			switch (lextable.table[i].lexema)
			{
			case LT_ID:		// Лексема - **идентификатор** (операнд или имя функции)
			case LT_LITERAL:	// Лексема - **литерал** (операнд)
				if (idtable.table[lextable.table[i].idxIT].idtype == IT::IDTYPE::F)
				{
					quantityParm = 0;
					function = true;
					result.push(lextable.table[i]); // Имя функции сразу отправляем в выходную очередь
					break;
				}
				// Если это операнд и мы внутри вызова функции,
				// и это первый операнд/параметр после имени функции
				if (function && !quantityParm)
					quantityParm++; // Увеличиваем счетчик параметров (для первого параметра)

				// Отправляем операнд (ID или LITERAL) в выходную очередь
				result.push(lextable.table[i]);
				break;

			case LT_OPERATION: // Лексема - **оператор** (+, -, *, / и т.д.)
				// Если стек пуст или на вершине стека левая скобка, просто помещаем оператор в стек
				if (stk.empty() || stk.top().lexema == LT_LEFTHESIS)
					stk.push(lextable.table[i]);
				else
				{
					// Определяем приоритет текущего оператора
					int prioritet = priority(lextable.table[i].sign);
					// Если приоритет оператора на вершине стека **больше или равен** приоритету текущего оператора
					if (priority(stk.top().sign) >= prioritet)
					{
						// Перемещаем оператор с вершины стека в выходную очередь
						result.push(stk.top());
						stk.pop(); // Удаляем оператор из стека
					}
					// Помещаем текущий оператор в стек
					stk.push(lextable.table[i]);
				}
				break;

			case LT_LEFTHESIS: // Лексема - **левая скобка** '('
				// Помещаем левую скобку в стек операторов
				stk.push(lextable.table[i]);
				break;

			case LT_RIGHTHESIS: // Лексема - **правая скобка** ')'
				// Перемещаем операторы из стека в очередь, пока не встретим левую скобку
				while (stk.top().lexema != LT_LEFTHESIS)
				{
					result.push(stk.top());
					stk.pop();
				}
				// Удаляем левую скобку из стека (не добавляя ее в очередь)
				stk.pop();

				// Если обрабатывался вызов функции
				if (function)
				{
					// Добавляем специальный маркер **@** для обозначения вызова функции
					LT::Entry a;
					a.lexema = '@';
					LT::Entry c;
					c.lexema = '0' + quantityParm;
					result.push(a);
					// Добавляем **количество параметров** (как лексему с символом '0' + quantityParm)
					result.push(c);
					function = false; // Сбрасываем флаг функции
				}
				break;

			case LT_COMMA: // Лексема - **запятая** (разделитель параметров функции)
				// Если мы внутри вызова функции, увеличиваем счетчик параметров
				if (function)
					quantityParm++;
				// Перемещаем операторы из стека в очередь до левой скобки
				// (операторы внутри параметра функции должны быть выполнены перед запятой)
				while (stk.top().lexema != LT_LEFTHESIS)
				{
					result.push(stk.top());
					stk.pop();
				}
				break;
			}
		}
		// После обработки всего выражения, перемещаем **оставшиеся операторы** из стека в очередь
		while (!stk.empty())
		{
			result.push(stk.top());
			stk.pop();
		}

		// Перезаписываем часть lextable (с lextable_pos до i) элементами из очереди result (ОПЗ)
		for (int j = lextable_pos; j < i; j++)
		{
			if (!result.empty())
			{
				// Заменяем лексему в таблице на лексему из ОПЗ
				lextable.table[j] = result.front();
				// Копируем номер строки (sn)
				lextable.table[j].sn = lextable.table[j - 1].sn;
				// Увеличиваем номер токена (tn)
				lextable.table[j].tn = lextable.table[j - 1].tn + 1;
				result.pop();
			}
			else
			{
				// Если очередь опустела раньше, чем закончился цикл, заполняем оставшееся
				// место специальными маркерами (например, @) или пустыми лексемами.
				// В данном случае, похоже, это добавление '@' в конец выражения, если результат короче исходного.
				lextable.table[j] = LT::Entry();
				lextable.table[j].lexema = '@';
				lextable.table[j].sn = lextable.table[j - 1].sn;
				lextable.table[j].tn = lextable.table[j - 1].tn + 1;
			}
		}

		return true; // Преобразование завершено
	}

	/**
	 * @brief Определяет приоритет оператора.
	 * @param v Символ оператора.
	 * @return Уровень приоритета (чем выше число, тем выше приоритет).
	 */
	int priority(char v)
	{
		switch (v)
		{
		case '(':
		case ')': return 1; // Самый низкий приоритет для скобок (внутри стека), чтобы они оставались там до ')'
		case ',': return 2; // Приоритет запятой
		case '+':
		case '-': return 3; // Приоритет сложения/вычитания
		case '*':
		case '/':
		case '%': return 4; // Самый высокий приоритет для умножения/деления/остатка
		default: return 0; // Для нераспознанных или других символов
		}
	}

	/**
	 * @brief Находит начало выражения (позицию после знака '=').
	 * @param lextable Таблица лексем.
	 * @return Индекс в lextable, следующий за знаком '=', или 0, если '=' не найден.
	 */
	int findExpression(LT::LexTable& lextable)
	{
		// Статическая переменная i сохраняет свое значение между вызовами,
		// что позволяет искать выражения последовательно, начиная с места, где остановился предыдущий поиск.
		for (static int i = 0; i < lextable.size; i++)
		{
			if (lextable.table[i].lexema == LT_EQUAL)
				return ++i; // Возвращаем позицию, следующую за '='
		}
		return 0; // Возвращаем 0, если больше присваиваний не найдено
	}
}
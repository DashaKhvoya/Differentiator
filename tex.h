#pragma once

FILE* fp_tex = nullptr;

const int simplify_messages_begin_count = 6;
const int simplify_messages_end_count = 4;

const int before_differentiation_messages_count = 4;
const int after_differentiation_messages_count = 4;


const char* intro_message_begin = "Продвинутые методы дифференцирования";
const char* initials_message = "Баранчикова Дарья";

const char* intro_message_description = "Производной называется предел отношения приращения\
 функции к приращению аргумента, когда приращение аргумента стремится к нулю";

const char* intro_message_end = "Давайте возьмем производную следующей функции:";


const char* outro_message = "Конечная производная данной функции равна:";


const char* literature_message = "Материал, использованный в отчете";
const char* references_message1 = "В.А.Зорич \"Математический анализ том 1\"";
const char* references_message2 = "Лекции, семинары и конспекты В.А.Редкозубова";


const char* before_differentiation_messages[] = {
    "Посчитаем производную для следующей функции:",
    "Возьмем производную функции:",
    "С легкостью можно взять произвонудю данной функции:",
    "Берем производную данной функции не успев моргнуть:"
};

const char* after_differentiation_messages[] = {
    "Очевидно, результатом дифференцирования будет:",
    "Получаем такой результат:",
    "Дереву ясно, что результатом является:",
    "С легкостью заметим, что получаем такой ответ:"
};


const char* before_simplify_message = "Давайте упростим";


const char* simplify_messages_begin[] = {
    "Ежу понятно, что",
    "И к бабке не ходи",
    "И ребёнку ясно, что",
    "К гадалке ходить не надо",
    "Ясно как дважды два четыре",
    "Комментарии излишни"
};

const char* simplify_messages_end[] = {
    "Равно",
    "Аналогично",
    "Эквивалентно",
    "Имеет такое же значение, как и"
};

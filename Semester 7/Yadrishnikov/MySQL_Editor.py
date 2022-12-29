from PyQt5 import QtWidgets, uic
from PyQt5.QtCore import QIODevice, QTimer
from PyQt5.QtWidgets import QTableWidget,QTableWidgetItem
import MySQLdb

# База данных
db = any
db_cur = any
db_opened = 0

# Загрузка приложения и его внешнего вида
app = QtWidgets.QApplication([])
ui = uic.loadUi("mysql.ui")
ui.setWindowTitle("Редактор MySQL")

ui.PB_DB_Close.setEnabled(0)
ui.PB_DB_Open.setEnabled(1)
ui.tabView.setEnabled(0)

def isNumber(string):
    if len(string) == 0:
        return False
    if string[0] == '-' and string[1:].isdigit():
        return True
    if string.isdigit():
        return True
    return False

def isUnsignedNumber(string):
    if len(string) == 0:
        return False
    if string[0:].isdigit():
        return True
    return False

# Возвращает список названий столбцов текущей выбранной таблицы
def getColumnNames():
    global db, db_cur, db_opened
    table_columns = []
    db_cur.execute("DESCRIBE " + ui.ViewTableBox.currentText() + ";")
    db_cur_data = db_cur.fetchall()
    for column_info in db_cur_data:
        table_columns.append(str(column_info).split('\'')[1])
    return table_columns

# Получить тип данных столбца текущей выбранной таблицы
def getColumnDataType(column):
    global db, db_cur, db_opened
    db_cur.execute("DESCRIBE " + ui.ViewTableBox.currentText() + ";")
    db_cur_data = db_cur.fetchall()
    
    # Определение типа данных столбца
    if str(db_cur_data[column]).find('int(') != -1:
        return 0
    if str(db_cur_data[column]).find('char(') != -1:
        return 1
    if str(db_cur_data[column]).find('date') != -1:
        return 2
    return -1

# Получить размер данных столбца текущей выбранной таблицы
def getColumnDataSize(column):
    global db, db_cur, db_opened
    db_cur.execute("DESCRIBE " + ui.ViewTableBox.currentText() + ";")
    db_cur_data = db_cur.fetchall()
    return int(str(db_cur_data[column]).split('(')[2].split(')')[0])

# Проверяет, является ли значение в столбце беззнаковым
def getColumnDataUnsigned(column):
    global db, db_cur, db_opened
    db_cur.execute("DESCRIBE " + ui.ViewTableBox.currentText() + ";")
    db_cur_data = db_cur.fetchall()
    if str(db_cur_data[column]).find('unsigned') != -1:
        return 1
    return 0

# Возвращает индекс столба, являющегося первичным ключом
def getPrimaryKeyColumn():
    global db, db_cur, db_opened
    db_cur.execute("DESCRIBE " + ui.ViewTableBox.currentText() + ";")
    db_cur_data = db_cur.fetchall()
    columnIndex = 0
    # Поиск столбца с первичным ключом
    for column_info in db_cur_data:
        if str(column_info).find('PRI') != -1:
            return columnIndex
        columnIndex += 1
    return -1

# Проверяет, является ли значение в столбце авто инкрементом
def getColumnDataIncrement(column):
    global db, db_cur, db_opened
    db_cur.execute("DESCRIBE " + ui.ViewTableBox.currentText() + ";")
    db_cur_data = db_cur.fetchall()
    if str(db_cur_data[column]).find('auto_increment') != -1:
        return 1
    return 0

# Обновление списка доступных таблиц
def UpdateTablesInfo():
    global db, db_cur, db_opened
    
    if db_opened == 0:
        return
    
    # Обновление списка доступных таблиц
    db_cur.execute("SHOW TABLES;")
    for tableName in db_cur.fetchall():
        ui.ViewTableBox.addItem(str(tableName).split('\'')[1])

# Добавить строку со значениями по умолчанию
def AddRowTable():
    global db, db_cur, db_opened

    if db_opened == 0:
        return

    sql_request="INSERT INTO " + ui.ViewTableBox.currentText() + " ("
    column_names = getColumnNames()


    j = 0
    for i in range(0, len(column_names)):
        if j != 0:
            sql_request += ", "
        if getColumnDataIncrement(i):
            continue
        sql_request += column_names[i]
        j += 1

    sql_request += ") VALUES ("

    j = 0
    for i in range(0, len(column_names)):
        if j != 0:
            sql_request += ", "
        if getColumnDataIncrement(i):
            continue
        if getColumnDataType(i) == 0:
            sql_request += "1"
        elif getColumnDataType(i) == 1:
            sql_request += "\"text\""
        elif getColumnDataType(i) == 2:
            sql_request += "\"2022-12-26\""
        j += 1

    sql_request += ");"

    try:
        db_cur.execute(sql_request)
        ui.statusBar.showMessage("Строка со значениями по умолчанию успешно добавлена")
    except Exception as E:
        ui.statusBar.showMessage("Невозможно добавить новую строку: " + str(E))
    
    UpdateTable()

# Удалить строку по номеру
def DeleteRowTable():
    global db, db_cur, db_opened

    if db_opened == 0:
        return

    sql_request="DELETE FROM " + ui.ViewTableBox.currentText() + " WHERE "
    column_names = getColumnNames()
    sql_request += column_names[getPrimaryKeyColumn()] + " = " + ui.tableWidget.item(ui.spinBoxRow.value() - 1, getPrimaryKeyColumn()).text()
    sql_request += ";"

    #print(sql_request)
    
    try:
        db_cur.execute(sql_request)
        ui.statusBar.showMessage("Строка успешно удалена")
    except Exception as E:
        ui.statusBar.showMessage("Невозможно удалить строку: " + str(E))
    
    UpdateTable()

# Добавить столбец
def AddColumnTable():
    global db, db_cur, db_opened

    if db_opened == 0:
        return

    if len(ui.ColumnName.text()) == 0:
        ui.statusBar.showMessage("Не указано имя столбца")
        return

    sql_request="ALTER TABLE " + ui.ViewTableBox.currentText() + " ADD "
    column_names = getColumnNames()
    sql_request += ui.ColumnName.text() + " " + ui.ColumnType.currentText() + ";"

    #print(sql_request)
    
    try:
        db_cur.execute(sql_request)
        ui.statusBar.showMessage("Столбец успешно добавлен")
    except Exception as E:
        ui.statusBar.showMessage("Невозможно добавить столбец: " + str(E))
    
    UpdateTable()

def DeleteColumnTable():
    global db, db_cur, db_opened

    if db_opened == 0:
        return
    
    sql_request="ALTER TABLE " + ui.ViewTableBox.currentText() + " DROP " + ui.ColumnNameBox.currentText() + ";"

    #print(sql_request)
    
    try:
        db_cur.execute(sql_request)
        ui.statusBar.showMessage("Столбец успешно удален")
    except Exception as E:
        ui.statusBar.showMessage("Невозможно удалить столбец: " + str(E))

    UpdateTable()

def tableValueUpdate(row, column):
    global db, db_cur, db_opened
    
    primarykey_column = getPrimaryKeyColumn()
    try:
        db_cur.execute("UPDATE " + ui.ViewTableBox.currentText() + " SET " + getColumnNames()[column] + " = \"" + ui.tableWidget.item(row, column).text() + "\" WHERE " + getColumnNames()[primarykey_column] + " = " + ui.tableWidget.item(row, primarykey_column).text() + ";")
        ui.statusBar.showMessage("Значение успешно изменено")
    except Exception as E:
        ui.statusBar.showMessage("Невозможно обновить значение: " + str(E))

# Если какой-то элемент таблицы был изменён
def tableValueChanged(row, column):
    global db, db_cur, db_opened
    
    if getColumnDataIncrement(column):
        ui.statusBar.showMessage("Нельзя изменять AUTO_INCREMENT столбцы")
        UpdateTable()
        return
    
    if getColumnDataType(column) == 0:
        if getColumnDataUnsigned(column) == 1:
            if isUnsignedNumber(ui.tableWidget.item(row, column).text()):
                tableValueUpdate(row, column)
            else:
                ui.statusBar.showMessage("Значение " + ui.tableWidget.item(row, column).text() + " не является беззнаковым целым")
        elif isNumber(ui.tableWidget.item(row, column).text()):
                tableValueUpdate(row, column)
        else:
            ui.statusBar.showMessage("Значение " + ui.tableWidget.item(row, column).text() + " не является целым числом")
    elif getColumnDataType(column) == 1:
        if len(str(ui.tableWidget.item(row, column).text())) < getColumnDataSize(column):
            tableValueUpdate(row, column)
        else:
            ui.statusBar.showMessage("Превышена максимальная длина стрки (" + str(getColumnDataSize(column)) + " символов)")
        
    # Обновление данных о таблице
    UpdateTable()

# Отрисовать таблицу
def UpdateTable():
    global db, db_cur, db_opened

    ui.tableWidget.disconnect()

    # Получение количества столбцов таблицы
    db_cur.execute("SHOW COLUMNS FROM " + ui.ViewTableBox.currentText() + ";")
    db_cur_data = db_cur.fetchall()
    ui.tableWidget.setColumnCount(len(db_cur_data))

    # Получение названий столбцов таблицы
    index=0
    for tableColumnName in db_cur_data:
        #print(str(tableColumnName))
        ui.tableWidget.setHorizontalHeaderItem(index, QTableWidgetItem(str(tableColumnName).split('\'')[1]))
        index+=1

    # Получение строк таблицы
    db_cur.execute("SELECT * FROM " + ui.ViewTableBox.currentText() + ";")
    db_cur_data = db_cur.fetchall()
    ui.tableWidget.setRowCount(len(db_cur_data))
    rowIndex=0
    for tableRow in db_cur_data:
        columnIndex=0
        for tableRowValue in str(tableRow).split(', '):
            if columnIndex == 0:
                tableRowValue = tableRowValue.split('(')[1] 
            if columnIndex == (ui.tableWidget.columnCount() - 1):
                tableRowValue = tableRowValue.split(')')[0]
            if len(tableRowValue.split('\'')) > 2:
                #print(tableRowValue + "is with kavichki")
                tableRowValue = tableRowValue.split('\'')[1]
            ui.tableWidget.setItem(rowIndex, columnIndex, QTableWidgetItem(tableRowValue))

            columnIndex+=1
        rowIndex+=1

    ui.spinBoxRow.setMinimum(1)
    ui.spinBoxRow.setMaximum(ui.tableWidget.rowCount())

    ui.ColumnNameBox.clear()
    colNames = getColumnNames()
    for col_name in colNames:
        ui.ColumnNameBox.addItem(col_name)

    # Подключение слота
    ui.tableWidget.cellChanged.connect(tableValueChanged)

# Открытие базы данных
def OpenDatabase():
    # Проверка имени базы данных
    if len(ui.dbNameLine.text()) == 0:
        ui.statusBar.showMessage("Не введено имя базы данных")
        return

    # Проверка имени пользователя
    if len(ui.userNameLine.text()) == 0:
        ui.statusBar.showMessage("Не введено имя пользователя")
        return

    global db, db_cur, db_opened
    try:
        # Попытка открыть базу данных
        db = MySQLdb.connect("localhost", ui.userNameLine.text(), ui.userPasswordLine.text(), ui.dbNameLine.text())
        
        # Если удалось открыть базу данных
        db_cur = db.cursor()
        ui.statusBar.showMessage("База данных \"" + ui.dbNameLine.text() + "\" успешно открыта")
        db_opened=1
        ui.PB_DB_Close.setEnabled(1)
        ui.PB_DB_Open.setEnabled(0)
        ui.tabView.setEnabled(1)

        UpdateTablesInfo()

    except Exception as E:

        # Если открыть базу данных не удалось
        ui.statusBar.showMessage("Не удалось открыть базу данных \"" + ui.dbNameLine.text() + "\": " + str(E))

def CloseDatabase():
    global db, db_cur, db_opened
    db_opened=0
    ui.PB_DB_Close.setEnabled(0)
    ui.PB_DB_Open.setEnabled(1)
    ui.tabView.setEnabled(0)
    ui.statusBar.showMessage("Вход в базу данных не выполнен")
    ui.ViewTableBox.clear()
    db.commit()
    db.close()



# Подключение сигналов к слотам
ui.PB_DB_Open.clicked.connect(OpenDatabase)
ui.PB_DB_Close.clicked.connect(CloseDatabase)
ui.ViewUpdateButton.clicked.connect(UpdateTable)
ui.AddRow.clicked.connect(AddRowTable)
ui.DeleteRow.clicked.connect(DeleteRowTable)
ui.AddColumn.clicked.connect(AddColumnTable)
ui.DeleteColumn.clicked.connect(DeleteColumnTable)
ui.statusBar.showMessage("Вход в базу данных не выполнен")

ui.show()
app.exec()
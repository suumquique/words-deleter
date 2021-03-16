## Удаление строк или слов из файла

Проект компилируется и запускается из командной строки таким образом: `executionFile.exe configFile.txt fileWithText.txt`

#### Вид конфигурационного файла (команды):

1. `^word` - удаление первого совпадения в тексте.
2. `^10word` - удаление первых десяти совпадений в тексте.
3. `#word` - удаление последнего совпадения в тексте.
4. `#10word` - удаление последних 10 совпадений в тексте.
5. `word` - удаление всех совпадений.

Вместо слова "word" может быть любое другое слово или предложение. Каждая команда для удаления должна заканчиваться переносом строки.

#### Вид текстового файла:

1. До 250 мегабайт (~260 миллионов байт, то есть символов Unicode)
2. Любое содержимое
3. Кодировка utf-8 без BOM

#### Нюансы работы программы:

Чтобы лишний раз не разворачивать весь текст из основного файла, сначала выполняются команды на удаление строк (слов) с начала файла, а затем команды на удаление строк (слов) с конца файла. По этой причине сохранение директив из конфигурационного файла идет в сортированный линейный список.

Из-за указанного выше фактора последовательность выполнения команд из конфигурационного файла может быть нарушена; в результате на выходе может получится отличающийся от ожидаемого текст, если команды частично перекрывали друг друга, как на примере ниже.



##### Пример

Допустим, имеется конфигурационный файл со следующими строками:

```
#10sit
Lorem ipsum dolor sit amet
```

И текстовый файл с таким содержанием:

```
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent placerat dapibus nisi non hendrerit.
Mauris id eros dignissim sem pulvinar aliquet id sagittis ligula. Nullam porta non turpis in congue. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
Quisque non enim non urna accumsan tempor ut a augue. Ut sodales orci diam, sed gravida diam ultrices eu. In nec dui at libero eleifend condimentum quis sed eros. Curabitur dapibus volutpat gravida.
```

Неверным будет думать, что сначала удалятся 10 слов `sit`, начиная с конца файла, а неполные строки вида `Lorem ipsum dolor  amet` останутся.  На самом деле, последовательность иная: команды добавятся в сортированный список, удаление `sit`, поскольку оно идет с конца файла, уйдет в низ стека, и первым выполнится удаление строк `Lorem ipsum dolor sit amet`. То есть, итоговый текст будет таким:

```
, consectetur adipiscing elit. Praesent placerat dapibus nisi non hendrerit. Mauris id eros dignissim sem pulvinar aliquet id sagittis ligula. 
Nullam porta non turpis in congue. , consectetur adipiscing elit. Quisque non enim non urna accumsan tempor ut a augue.
Ut sodales orci diam, sed gravida diam ultrices eu. In nec dui at libero eleifend condimentum quis sed eros. Curabitur dapibus volutpat gravida.
```


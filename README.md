# hwp2pdf

This console application converts HWP document into PDF document, using [pyhwp](https://github.com/mete0r/pyhwp) tool and [tinyXML2](https://github.com/leethomason/tinyxml2) Library

## System Architecture
```
   INPUT                          OUTPUT #1                        OUTPUT #2
------------    \---------/    ----------------    \---------/    ------------
|   HWP    | ->  | pyhwp |  -> | BodyText.xml | ->  | xh2pdf|  -> |   PDF    |
------------    /---------\    ----------------    /---------\    ------------
```

## Structure of BodyText.xml
```
<BodyText>
└---<SectionDef>
        └---<PageDef>
        └---<FootnoteShape>
        └---<PageBorderFill>
        └---<Paragraph>
                └---<LineSeg>
                        └---<ColumnsDef>
                        └---<Text>
                                └---<Image>
	 	        └---<ControlChar>
```
| Tag           | Description |
|---------------|-------------|
| BodyText      | whole contents of HWP document |
| SectionDef    | design properties of document page |
|               | (e.g. default tab stops, column spacing, hide header/footer, starting page number) |
| PageDef       | layout properties of document page |
| FootnoteShape | note informations |
| PageBorderFill| header and footer informations |
| Paragraph     | paragraph informations (parted as sentence) |
| LineSeg       | line segment informations (parted line by line) |
| ColumnsDef    | splitter informations |
| Text          | text contents |
| Image         | image contents |
| ControlChar   | character information for splitting paragraph |

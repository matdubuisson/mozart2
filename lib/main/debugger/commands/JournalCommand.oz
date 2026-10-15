local
  proc {DisplayOptions}
    {DisplayFrame "Journal command without options" [
      "<condition>" "no arguments taken only an optional filtering condition"
    ]}
  end

  proc {DisplayJournal Arguments}
    Conditions = {ExtractConditions Arguments $}
  in
    if Conditions \= none then
      Journal = {Boot_EventManager.getEventsJournal $}
    in
      {DisplayJournalCSV
        [
          "Type            " "Category        "
          "Author Id" "Victim Id"
        ]
        Journal
        proc {$ Type Category Event ?Result}
          case Event of event(
            authorId: AuthorId
            victimId: VictimId
          ) then
            Result = event(
              type: Type
              category: Category
              authorId: AuthorId
              victimId: VictimId
            )
          end
        end
        proc {$ Event ?Result}
          Result = {CheckConditions Conditions Event $}
        end
        proc {$ Event ?Result}
          case Event of event(
            type: Type
            category: Category
            authorId: AuthorId
            victimId: VictimId
          ) then
            Result = [
              {Atom.toString Type $}
              {Atom.toString Category $}
              case AuthorId of none then "none"
              else {Int.toString AuthorId $} end
              {Int.toString VictimId $}
            ]
          end
        end}
    end
  end
in
  case Arguments of nil then
    {DisplayJournal nil}
  [] "help"|_ then
    {DisplayOptions}
  else
    {DisplayJournal Arguments}
  end
end
